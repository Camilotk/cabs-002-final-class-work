#include "PersistenceDAO.h"
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

bool PersistenceDAO::saveFrames(const std::vector<sf::Image>& frames,
                                const std::string& dirPath,
                                const std::string& prefix,
                                int startIndex) const {
    if (frames.empty()) {
        std::cerr << "[PersistenceDAO] Nenhum frame para salvar." << '\n';
        return false;
    }
    std::error_code ec;
    fs::create_directories(dirPath, ec);
    if (ec) {
        std::cerr << "[PersistenceDAO] Falha ao criar diretório: " << dirPath << " -> " << ec.message() << '\n';
        return false;
    }
    bool ok = true;
    int idx = startIndex;
    for (const auto& img : frames) {
        std::ostringstream fname;
        fname << prefix << '_' << std::setw(4) << std::setfill('0') << idx++ << ".png";
        auto filePath = (fs::path(dirPath) / fname.str()).string();
        if (!img.saveToFile(filePath)) {
            std::cerr << "[PersistenceDAO] Falha ao salvar " << filePath << '\n';
            ok = false; // continua salvando os demais
        }
    }
    std::cout << "[PersistenceDAO] " << (ok ? "Todos" : "Alguns") << " frames salvos em " << dirPath << '\n';
    return ok;
}

bool PersistenceDAO::exportMP4(const std::string& framesDir,
                               const std::string& outputFile,
                               int fps) const {
    fs::path pattern = fs::path(framesDir) / "frame_%04d.png";
    std::ostringstream cmd;
    cmd << "ffmpeg -y -framerate " << fps
        << " -i " << '"' << pattern.string() << '"'
        << " -c:v libx264 -pix_fmt yuv420p " << '"' << outputFile << '"';
    std::cout << "[PersistenceDAO] Executando: " << cmd.str() << '\n';
    int code = std::system(cmd.str().c_str());
    if (code != 0) {
        std::cerr << "[PersistenceDAO] ffmpeg retornou código " << code << '\n';
        return false;
    }
    std::cout << "[PersistenceDAO] Vídeo exportado: " << outputFile << '\n';
    return true;
}

bool PersistenceDAO::clearTempFiles(const std::string& framesDir,
                                    const std::string& prefix) const {
    std::error_code ec;
    if (!fs::exists(framesDir)) return true;
    size_t removed = 0;
    for (auto& entry : fs::directory_iterator(framesDir, ec)) {
        if (ec) break;
        if (entry.is_regular_file()) {
            auto name = entry.path().filename().string();
            if (name.rfind(prefix + '_', 0) == 0 && entry.path().extension() == ".png") {
                fs::remove(entry.path(), ec);
                if (!ec) ++removed;
            }
        }
    }
    if (ec) {
        std::cerr << "[PersistenceDAO] Erro ao remover arquivos: " << ec.message() << '\n';
        return false;
    }
    std::cout << "[PersistenceDAO] Removidos " << removed << " frames em " << framesDir << '\n';
    return true;
}
