#include "PersistenceDAO.h"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

bool PersistenceDAO::saveFrames(
    const std::vector<sf::Image> &frames, const std::string &dirPath,
    const std::string &prefix, int startIndex,
    const std::function<void(size_t, size_t)> &onProgress,
    const std::function<bool()> &shouldCancel) const {
  if (frames.empty()) {
    std::cerr << "[PersistenceDAO] Nenhum frame para salvar." << '\n';
    return false;
  }
  std::error_code ec;
  fs::create_directories(dirPath, ec);
  if (ec) {
    std::cerr << "[PersistenceDAO] Falha ao criar diretório: " << dirPath
              << " -> " << ec.message() << '\n';
    return false;
  }
  bool ok = true;
  int idx = startIndex;
  size_t total = frames.size();
  size_t current = 0;
  for (const auto &img : frames) {
    if (shouldCancel && shouldCancel()) {
      std::cout << "[PersistenceDAO] Cancelado salvamento de frames." << '\n';
      break;
    }
    std::ostringstream fname;
    fname << prefix << '_' << std::setw(4) << std::setfill('0') << idx++
          << ".png";
    auto filePath = (fs::path(dirPath) / fname.str()).string();
    if (!img.saveToFile(filePath)) {
      std::cerr << "[PersistenceDAO] Falha ao salvar " << filePath << '\n';
      ok = false; // continua salvando os demais
    }
    ++current;
    if (onProgress)
      onProgress(current, total);
  }
  std::cout << "[PersistenceDAO] " << (ok ? "Todos" : "Alguns")
            << " frames salvos em " << dirPath << '\n';
  return ok;
}

bool PersistenceDAO::exportMP4(const std::string &framesDir,
                               const std::string &outputFile, int fps) const {
  fs::path pattern = fs::path(framesDir) / "frame_%04d.png";
  std::ostringstream cmd;
  cmd << "ffmpeg -y -framerate " << fps << " -i " << '"' << pattern.string()
      << '"' << " -c:v libx264 -pix_fmt yuv420p " << '"' << outputFile << '"';
  std::cout << "[PersistenceDAO] Executando: " << cmd.str() << '\n';
  int code = std::system(cmd.str().c_str());
  if (code != 0) {
    std::cerr << "[PersistenceDAO] ffmpeg retornou código " << code << '\n';
    return false;
  }
  std::cout << "[PersistenceDAO] Vídeo exportado: " << outputFile << '\n';
  return true;
}

bool PersistenceDAO::exportMP4WithProgress(
    const std::string &framesDir, const std::string &outputFile, int fps,
    const std::function<void(const std::string &)> &onProgress) const {
  namespace fs = std::filesystem;
  fs::path pattern = fs::path(framesDir) / "frame_%04d.png";
  std::ostringstream cmd;

  cmd << "ffmpeg -y -hide_banner -loglevel error -progress pipe:1 -framerate "
      << fps << " -i " << '"' << pattern.string() << '"'
      << " -c:v libx264 -pix_fmt yuv420p " << '"' << outputFile << '"';

  std::string full = cmd.str();
  std::cout << "[PersistenceDAO] Executando (progress): " << full << '\n';
  FILE *pipe = popen(full.c_str(), "r");

  if (!pipe) {
    std::cerr << "[PersistenceDAO] Falha ao abrir pipe para ffmpeg." << '\n';
    return false;
  }

  std::string line;
  std::string aggregated;
  char buf[256];
  while (fgets(buf, sizeof(buf), pipe)) {
    line = buf;
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
      line.pop_back();

    if (line.empty())
      continue;

    if (onProgress)
      onProgress(line);
  }

  int code = pclose(pipe);

  if (code != 0) {
    std::cerr << "[PersistenceDAO] ffmpeg retornou código " << code << '\n';
    return false;
  }

  if (onProgress)
    onProgress("done");

  std::cout << "[PersistenceDAO] Vídeo exportado: " << outputFile << '\n';
  return true;
}

bool PersistenceDAO::exportMP4WithProgressCancelable(
    const std::string &framesDir, const std::string &outputFile, int fps,
    pid_t &outPid,
    const std::function<void(const std::string &)> &onProgress) const {

  namespace fs = std::filesystem;
  fs::path pattern = fs::path(framesDir) / "frame_%04d.png";

  std::vector<std::string> args = {"ffmpeg",
                                   "-y",
                                   "-hide_banner",
                                   "-loglevel",
                                   "error",
                                   "-progress",
                                   "pipe:1",
                                   "-framerate",
                                   std::to_string(fps),
                                   "-i",
                                   pattern.string(),
                                   "-c:v",
                                   "libx264",
                                   "-pix_fmt",
                                   "yuv420p",
                                   outputFile};

  int pipefd[2];

  if (pipe(pipefd) != 0) {

    std::cerr << "[PersistenceDAO] Falha pipe." << std::endl;
    return false;
  }

  pid_t pid = fork();

  if (pid < 0) {
    std::cerr << "[PersistenceDAO] fork falhou." << std::endl;
    return false;
  }

  if (pid == 0) {
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    dup2(pipefd[1], STDERR_FILENO);
    std::vector<char *> cargs;
    cargs.reserve(args.size() + 1);

    for (auto &a : args)
      cargs.push_back(const_cast<char *>(a.c_str()));

    cargs.push_back(nullptr);
    execvp("ffmpeg", cargs.data());
    _exit(127);
  }

  outPid = pid;
  close(pipefd[1]);
  FILE *stream = fdopen(pipefd[0], "r");

  if (!stream) {
    std::cerr << "[PersistenceDAO] fdopen falhou." << std::endl;
    return false;
  }

  char buf[256];
  while (true) {

    if (fgets(buf, sizeof(buf), stream) == nullptr) {
      if (feof(stream))
        break;
      else
        continue;
    }

    std::string line(buf);

    while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
      line.pop_back();

    if (!line.empty() && onProgress)
      onProgress(line);
  }

  fclose(stream);
  int status = 0;
  waitpid(pid, &status, 0);

  if (onProgress)
    onProgress("progress=end");

  return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

bool PersistenceDAO::cancelProcess(pid_t pid) {
  if (pid <= 0)
    return false;

  int r = kill(pid, SIGTERM);
  if (r != 0) {
    perror("kill");
    return false;
  }

  return true;
}

bool PersistenceDAO::clearTempFiles(const std::string &framesDir,
                                    const std::string &prefix) const {
  std::error_code ec;
  if (!fs::exists(framesDir))
    return true;
  size_t removed = 0;
  for (auto &entry : fs::directory_iterator(framesDir, ec)) {
    if (ec)
      break;
    if (entry.is_regular_file()) {
      auto name = entry.path().filename().string();
      if (name.rfind(prefix + '_', 0) == 0 &&
          entry.path().extension() == ".png") {
        fs::remove(entry.path(), ec);
        if (!ec)
          ++removed;
      }
    }
  }
  if (ec) {
    std::cerr << "[PersistenceDAO] Erro ao remover arquivos: " << ec.message()
              << '\n';
    return false;
  }
  std::cout << "[PersistenceDAO] Removidos " << removed << " frames em "
            << framesDir << '\n';
  return true;
}
