#include "Command.h"
#include "CommandRecorder.h"
#include "LinkedListVisualizer.h"
#include "RandomProvider.h"
#include "StructureController.h"
#include "StructureFactory.h"
#include "VectorVisualizer.h"
#include <SFML/Graphics.hpp>
#include <atomic>
#include <ctime>
#include <future>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

void drawCommandPanel(sf::RenderWindow &window, sf::Font &font,
                      const VectorVisualizer &vecViz,
                      const LinkedListVisualizer &listViz) {

  const float panelWidth = 280.f;
  const float panelX = window.getSize().x - panelWidth;

  sf::RectangleShape panel(sf::Vector2f(panelWidth, window.getSize().y));
  panel.setPosition(panelX, 0);
  panel.setFillColor(sf::Color(20, 20, 20, 200));
  window.draw(panel);

  sf::Text title("Fila de Comandos Pendentes", font, 20);
  title.setFillColor(sf::Color::White);
  title.setPosition(panelX + 15, 15);
  window.draw(title);

  const auto &vecQueue = vecViz.getOperationQueue();
  const auto &listQueue = listViz.getOperationQueue();

  std::vector<std::string> allCommands;
  for (const auto &cmd : vecQueue)
    allCommands.push_back(cmd.description);
  for (const auto &cmd : listQueue)
    allCommands.push_back(cmd.description);

  sf::Text commandText("", font, 16);
  commandText.setFillColor(sf::Color(220, 220, 220));
  float currentY = 60.f;

  if (allCommands.empty()) {
    commandText.setString("(vazio)");
    commandText.setPosition(panelX + 15, currentY);
    window.draw(commandText);
  } else {
    for (const auto &desc : allCommands) {
      commandText.setString(desc);
      commandText.setPosition(panelX + 15, currentY);
      window.draw(commandText);
      currentY += 25.f;
    }
  }

  sf::Text stats("Pendentes: " + std::to_string(allCommands.size()), font, 14);
  stats.setFillColor(sf::Color(180, 180, 180));
  stats.setPosition(panelX + 15, window.getSize().y - 30.f);
  window.draw(stats);
}

static const std::vector<std::pair<std::string, std::string>> COMMAND_HELP = {
    {"I", "Inserir elemento aleatorio no Vetor"},
    {"R", "Remover elemento (indice aleatorio) do Vetor"},
    {"H", "Destacar elemento (indice aleatorio) no Vetor"},
    {"V", "Limpar todos os elementos do Vetor"},
    {"A", "Inserir elemento aleatorio na Lista"},
    {"D", "Remover elemento (posicao aleatoria) da Lista"},
    {"B", "Limpar todos os elementos da Lista"},
    {"F", "Toggle capturar frames em memoria"},
    {"C", "Limpar frames em memoria"},
    {"X", "Limpar frames salvos em disco"},
    {"E", "Exportar frames em PNG (frames/vector)"},
    {"M", "Exportar MP4 (vector.mp4) via ffmpeg"},
    {"G", "Iniciar/Parar gravacao de comandos"},
    {"S", "Salvar comandos gravados em texto (commands.log)"},
    {"L", "Carregar e executar replay imediato (commands.log)"},
    {"J", "Salvar comandos em JSON (commands.json)"},
    {"K", "Carregar JSON e iniciar replay temporal"},
    {"P", "Pausar/Retomar replay temporal"},
    {"N", "Avancar um passo no replay quando pausado"},
    {"[", "Diminuir velocidade do replay temporal"},
    {"]", "Aumentar velocidade do replay temporal"},
    {"B", "(Futuro) Toggle modo circular de frames"},
    {"T", "Toggle limite de frames (900 <-> 1800)"}};

int main() {
  sf::RenderWindow window(sf::VideoMode(1400, 800),
                          "Visualizador Animado de Estruturas de Dados");
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("arial.ttf")) {
    std::cerr << "Erro ao carregar fonte\n";
    return -1;
  }

  sf::Text titleText("Visualizador de Estruturas de Dados", font, 24);
  sf::Text instructionsText;

  VectorVisualizer vecViz(font, {50.f, 150.f});
  LinkedListVisualizer listViz(font, {50.f, 400.f});
  RandomProvider rng;

  StructureFactory factory;
  auto arrayListStructure = factory.create("array_list");
  auto linkedListStructure = factory.create("linked_list");

  StructureController controllerArray(std::move(arrayListStructure), &vecViz,
                                      &rng);
  StructureController controllerList(std::move(linkedListStructure), &listViz,
                                     &rng);
  controllerArray.connect();
  controllerList.connect();

  sf::Clock clock;

  CommandRecorder recorder;
  bool showLimitStatus = false;
  const std::string recordFile = "commands.log";
  const std::string recordJSON = "commands.json";

  // Replay temporal
  bool timedReplayActive = false;
  size_t timedReplayIndex = 0;
  float timedReplayClock = 0.f;
  float timedReplaySpeed = 1.f;
  bool timedReplayPaused = false;

  struct Subtitle {
    std::string text;
    float age;
  };
  std::vector<Subtitle> subtitles;
  auto pushSubtitle = [&subtitles](const std::string &t) {
    subtitles.push_back({t, 0.f});
    if (subtitles.size() > 12)
      subtitles.erase(subtitles.begin());
  };
  const float SUBTITLE_DURATION = 3.5f;

  sf::RectangleShape helpButton(sf::Vector2f(110.f, 30.f));
  helpButton.setFillColor(sf::Color(60, 60, 140));
  helpButton.setOutlineColor(sf::Color(180, 180, 255));
  helpButton.setOutlineThickness(1.f);

  bool showHelpWindow = false;
  std::unique_ptr<sf::RenderWindow> helpWindow;

  bool exportingFrames = false;
  std::atomic<size_t> framesProgressCurrent{0};
  std::atomic<size_t> framesProgressTotal{0};
  sf::Clock exportClock;
  bool videoCancelRequested = false;
  bool framesCancelRequested = false;
  pid_t currentVideoPid = -1;
  bool exportingVideo = false;
  std::future<bool> exportFutureFrames;
  std::atomic<bool> videoDone{false};
  std::mutex videoProgressMutex;
  std::string videoProgressLine;

  while (window.isOpen()) {
    sf::Time elapsed = clock.restart();
    float dt = elapsed.asSeconds();

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0.f, 0.f, event.size.width,
                                  event.size.height);
        window.setView(sf::View(visibleArea));
      }

      if (event.type == sf::Event::MouseMoved) {
        helpButton.setPosition(15.f, 100.f);
        if (helpButton.getGlobalBounds().contains(
                static_cast<float>(event.mouseMove.x),
                static_cast<float>(event.mouseMove.y))) {
          helpButton.setFillColor(sf::Color(80, 80, 180));
        } else {
          helpButton.setFillColor(sf::Color(60, 60, 140));
        }
      }

      if (event.type == sf::Event::MouseButtonPressed &&
          event.mouseButton.button == sf::Mouse::Left) {
        if (helpButton.getGlobalBounds().contains(
                static_cast<float>(event.mouseButton.x),
                static_cast<float>(event.mouseButton.y))) {
          if (!showHelpWindow) {
            helpWindow = std::make_unique<sf::RenderWindow>(
                sf::VideoMode(600, 700), "Comandos Disponiveis",
                sf::Style::Titlebar | sf::Style::Close);
            helpWindow->setFramerateLimit(60);
            showHelpWindow = true;
          } else {
            if (helpWindow && helpWindow->isOpen()) {
              helpWindow->close();
            }
            showHelpWindow = false;
          }
        }
      }

      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::I)
          controllerArray.executeAndRecord("insert", &recorder, "vector");
        else if (event.key.code == sf::Keyboard::R)
          controllerArray.executeAndRecord("remove", &recorder, "vector");
        else if (event.key.code == sf::Keyboard::A)
          controllerList.executeAndRecord("insert", &recorder, "list");
        else if (event.key.code == sf::Keyboard::D)
          controllerList.executeAndRecord("remove", &recorder, "list");
        else if (event.key.code == sf::Keyboard::H)
          controllerArray.executeAndRecord("highlight", &recorder, "vector");
        else if (event.key.code == sf::Keyboard::V)
          controllerArray.executeAndRecord("clear", &recorder, "vector");
        else if (event.key.code == sf::Keyboard::B)
          controllerList.executeAndRecord("clear", &recorder, "list");
        else if (event.key.code == sf::Keyboard::E) {
          if (!exportingFrames) {
            exportingFrames = true;
            framesProgressCurrent = 0;
            framesProgressTotal = vecViz.getCapturedFrameCount();
            pushSubtitle("Export PNG iniciada");
            exportFutureFrames =
                std::async(std::launch::async, [&vecViz, &framesProgressCurrent,
                                                &framesProgressTotal,
                                                &framesCancelRequested]() {
                  vecViz.exportFramesWithProgress(
                      "frames/vector",
                      [&framesProgressCurrent,
                       &framesProgressTotal](size_t cur, size_t total) {
                        framesProgressCurrent = cur;

                        framesProgressTotal = total;
                      },
                      [&framesCancelRequested]() {
                        return framesCancelRequested;
                      });

                  return true;
                });
          }
        } else if (event.key.code == sf::Keyboard::M) {
          if (!exportingVideo) {
            exportingVideo = true;
            videoDone = false;
            videoProgressLine.clear();
            videoCancelRequested = false;
            exportClock.restart();
            pushSubtitle("Export MP4 iniciada");
            std::thread([&vecViz, &videoDone, &videoProgressLine,
                         &videoProgressMutex, &currentVideoPid]() {
              const int fpsLocal = 30;
              size_t totalFrames = vecViz.getCapturedFrameCount();

              if (totalFrames == 0)
                totalFrames = vecViz.getCaptureLimit();

              std::atomic<int> curFrame{-1};
              std::atomic<long long> outTimeMs{-1};
              std::atomic<bool> finished{false};

              PersistenceDAO dao;
              dao.exportMP4WithProgressCancelable(
                  "frames/vector", "vector.mp4", fpsLocal, currentVideoPid,
                  [&videoDone, &videoProgressLine, &videoProgressMutex,
                   &curFrame, &outTimeMs, &finished,
                   totalFrames](const std::string &raw) {
                    if (raw == "progress=end") {
                      finished = true;
                      videoDone = true;
                      return;
                    }

                    auto eqPos = raw.find('=');
                    if (eqPos == std::string::npos)
                      return;

                    std::string key = raw.substr(0, eqPos);
                    std::string val = raw.substr(eqPos + 1);

                    if (key == "frame") {
                      try {
                        curFrame = std::stoi(val);
                      } catch (...) {
                      }
                    } else if (key == "out_time_ms") {
                      try {
                        outTimeMs = std::stoll(val);
                      } catch (...) {
                      }
                    }

                    if (curFrame.load() >= 0) {
                      double pct = (totalFrames > 0)
                                       ? (double)curFrame.load() /
                                             (double)totalFrames * 100.0
                                       : 0.0;
                      double etaSec = 0.0;
                      if (totalFrames > 0 && curFrame.load() >= 0) {
                        double remainingFrames =
                            (double)totalFrames - curFrame.load();
                        etaSec = remainingFrames / fpsLocal;
                      }

                      int etaInt = (int)etaSec;
                      int etaM = etaInt / 60;
                      int etaS = etaInt % 60;

                      double curTimeSec =
                          (outTimeMs.load() >= 0)
                              ? (double)outTimeMs.load() / 1000000.0
                              : -1.0;
                      char buf[160];
                      if (curTimeSec >= 0) {
                        double totalExpectedSec =
                            (totalFrames > 0) ? (double)totalFrames / fpsLocal
                                              : 0.0;
                        double pctTime =
                            (totalExpectedSec > 0)
                                ? curTimeSec / totalExpectedSec * 100.0
                                : 0.0;
                        snprintf(buf, sizeof(buf),
                                 "frame %d/%zu (%.1f%%) time %.2fs (%.1f%%) "
                                 "ETA %02d:%02d",
                                 curFrame.load(), totalFrames, pct, curTimeSec,
                                 pctTime, etaM, etaS);
                      } else {
                        snprintf(buf, sizeof(buf),
                                 "frame %d/%zu (%.1f%%) ETA %02d:%02d",
                                 curFrame.load(), totalFrames, pct, etaM, etaS);
                      }

                      std::lock_guard<std::mutex> lk(videoProgressMutex);
                      videoProgressLine = buf;
                    } else {
                      std::lock_guard<std::mutex> lk(videoProgressMutex);
                      videoProgressLine = raw;
                    }
                  });
              videoDone = true;
            }).detach();
          }
        } else if (event.key.code == sf::Keyboard::Z) {
          if (exportingVideo && !videoDone && currentVideoPid > 0) {
            if (PersistenceDAO::cancelProcess(currentVideoPid)) {
              pushSubtitle("Video cancelado");
              videoCancelRequested = true;
            } else {
              pushSubtitle("Falha cancelar video");
            }
          }

          if (exportingFrames) {
            framesCancelRequested = true;
            pushSubtitle("Frames cancelados (parar apos atual)");
          }
        } else if (event.key.code == sf::Keyboard::F) {
          vecViz.toggleCapture();
          pushSubtitle(vecViz.isCaptureEnabled() ? "Captura ON"
                                                 : "Captura OFF");
        } else if (event.key.code == sf::Keyboard::C) {
          vecViz.clearMemoryFrames();
          pushSubtitle("Frames memoria limpos");
        } else if (event.key.code == sf::Keyboard::X) {
          vecViz.clearSavedFrames("frames/vector");
          pushSubtitle("Frames disco limpos");
        } else if (event.key.code == sf::Keyboard::G) {
          bool wasRecording = recorder.isRecording();
          recorder.toggle();
          if (!wasRecording && recorder.isRecording()) {
            framesCancelRequested = false;
            exportClock.restart();
            if (!rng.hasSeed())
              rng.setSeed(recorder.seed());
            pushSubtitle("Seed=" + std::to_string(recorder.seed()));
          }
          pushSubtitle(recorder.isRecording() ? "Gravacao ON" : "Gravacao OFF");
        } else if (event.key.code == sf::Keyboard::S) {
          if (recorder.save(recordFile))
            std::cout << "[Recorder] Comandos salvos em " << recordFile << "\n";
          pushSubtitle("Salvar LOG");
        } else if (event.key.code == sf::Keyboard::L) {
          if (recorder.load(recordFile)) {
            std::cout << "[Recorder] Replay imediato iniciando...\n";
            for (const auto &cmd : recorder.get()) {
              if (cmd.target == "vector") {
                if (cmd.op == "INSERT" && cmd.hasValue)
                  controllerArray.insertAt(cmd.index, cmd.value);
                else if (cmd.op == "REMOVE")
                  controllerArray.removeAt(cmd.index);
                else if (cmd.op == "HIGHLIGHT")
                  controllerArray.highlightAt(cmd.index);
                pushSubtitle("Replay:" + cmd.op + " vector");
              } else if (cmd.target == "list") {
                if (cmd.op == "INSERT" && cmd.hasValue)
                  controllerList.insertAt(cmd.index, cmd.value);
                else if (cmd.op == "REMOVE")
                  controllerList.removeAt(cmd.index);
                else if (cmd.op == "HIGHLIGHT")
                  controllerList.highlightAt(cmd.index);
                pushSubtitle("Replay:" + cmd.op + " list");
              }
            }
            std::cout << "[Recorder] Replay imediato finalizado." << std::endl;
            pushSubtitle("Replay imediato fim");
          }
        } else if (event.key.code == sf::Keyboard::J) {
          if (recorder.saveJSON(recordJSON))
            std::cout << "[Recorder] JSON salvo em " << recordJSON << "\n";
          pushSubtitle("Salvar JSON");
        } else if (event.key.code == sf::Keyboard::K) {
          if (recorder.loadJSON(recordJSON)) {
            std::cout << "[Recorder] Replay temporal carregado de JSON...\n";
            if (recorder.seed() &&
                (!rng.hasSeed() || rng.seed() != recorder.seed())) {
              rng.setSeed(recorder.seed());
              pushSubtitle("Seed aplicada=" + std::to_string(recorder.seed()));
            }
            timedReplayActive = true;
            timedReplayIndex = 0;
            timedReplayClock = 0.f; // reinicia
            timedReplayPaused = false;
            timedReplaySpeed = 1.f;
            pushSubtitle("Replay temporal ON");
          }
        } else if (event.key.code == sf::Keyboard::P) {
          if (timedReplayActive)
            timedReplayPaused = !timedReplayPaused;
          pushSubtitle(timedReplayPaused ? "Replay PAUSE" : "Replay RESUME");
        } else if (event.key.code == sf::Keyboard::N) {
          if (timedReplayActive && timedReplayPaused) {
            timedReplayClock = recorder.get().empty()
                                   ? 0.f
                                   : recorder.get()[timedReplayIndex].t;
            timedReplayPaused = false;
            pushSubtitle("Replay STEP");
          }
        } else if (event.key.code == sf::Keyboard::LBracket) {
          timedReplaySpeed = std::max(0.1f, timedReplaySpeed * 0.5f);
          std::cout << "[Replay] speed=" << timedReplaySpeed << '\n';
          pushSubtitle("Replay speed=" + std::to_string(timedReplaySpeed));
        } else if (event.key.code == sf::Keyboard::RBracket) {
          timedReplaySpeed = std::min(16.f, timedReplaySpeed * 2.f);
          std::cout << "[Replay] speed=" << timedReplaySpeed << '\n';
          pushSubtitle("Replay speed=" + std::to_string(timedReplaySpeed));
        } else if (event.key.code == sf::Keyboard::B) {
          vecViz.toggleCapture();
          std::cout << "[Frames] Modo circular requer implementacao completa "
                       "(placeholder)."
                    << '\n';
          pushSubtitle("Modo circular placeholder");
        } else if (event.key.code == sf::Keyboard::T) {
          size_t currentLimit = vecViz.getCaptureLimit();
          size_t newLimit = (currentLimit <= 900) ? 1800 : 900;
          vecViz.setCaptureMaxFrames(newLimit);
          std::cout << "[Frames] Limite de captura agora = " << newLimit
                    << " frames." << '\n';
          showLimitStatus = true;
          pushSubtitle("Limite=" + std::to_string(newLimit));
        }
      }
    }

    vecViz.update(dt);
    listViz.update(dt);

    if (exportingFrames && exportFutureFrames.valid() &&
        exportFutureFrames.wait_for(std::chrono::milliseconds(0)) ==
            std::future_status::ready) {
      if (framesCancelRequested) {
        pushSubtitle("Export PNG cancelada em " +
                     std::to_string(framesProgressCurrent.load()) + " frames");
      } else {
        pushSubtitle("Export PNG concluida");
      }
      exportingFrames = false;
    }

    if (exportingVideo && videoDone.load()) {
      if (videoCancelRequested) {
        pushSubtitle("Export MP4 cancelada");
      } else {
        pushSubtitle("Export MP4 concluida");
      }
      exportingVideo = false;
    }

    if (timedReplayActive) {
      if (!timedReplayPaused)
        timedReplayClock += dt * timedReplaySpeed;
      const auto &cmds = recorder.get();
      while (timedReplayIndex < cmds.size() &&
             cmds[timedReplayIndex].t <= timedReplayClock) {
        const auto &cmd = cmds[timedReplayIndex];
        if (cmd.target == "vector") {
          if (cmd.op == "INSERT" && cmd.hasValue)
            controllerArray.insertAt(cmd.index, cmd.value);
          else if (cmd.op == "REMOVE")
            controllerArray.removeAt(cmd.index);
          else if (cmd.op == "HIGHLIGHT")
            controllerArray.highlightAt(cmd.index);
          pushSubtitle("Temporal:" + cmd.op + " vector");
        } else if (cmd.target == "list") {
          if (cmd.op == "INSERT" && cmd.hasValue)
            controllerList.insertAt(cmd.index, cmd.value);
          else if (cmd.op == "REMOVE")
            controllerList.removeAt(cmd.index);
          else if (cmd.op == "HIGHLIGHT")
            controllerList.highlightAt(cmd.index);
          pushSubtitle("Temporal:" + cmd.op + " list");
        }
        timedReplayIndex++;
      }
      if (timedReplayIndex >= cmds.size()) {
        timedReplayActive = false;
        std::cout << "[Recorder] Replay temporal concluido." << std::endl;
        pushSubtitle("Replay temporal fim");
      }
    }

    window.clear(sf::Color(30, 30, 30));

    titleText.setPosition(window.getSize().x / 2.0f, 30.f);

    std::string instructionsString =
        "[I] Inserir no Vetor  |  [R] Remover do Vetor\n"
        "[A] Adicionar na Lista  |  [D] Remover da Lista";
    instructionsText.setFont(font);
    instructionsText.setString(instructionsString);
    instructionsText.setCharacterSize(16);
    instructionsText.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect instructionsBounds = instructionsText.getLocalBounds();
    instructionsText.setOrigin(
        instructionsBounds.left + instructionsBounds.width / 2.0f,
        instructionsBounds.top + instructionsBounds.height / 2.0f);
    instructionsText.setPosition(window.getSize().x / 2.0f, 70.f);

    window.draw(titleText);
    window.draw(instructionsText);

    window.draw(helpButton);
    sf::Text helpButtonText("Ajuda", font, 16);
    helpButtonText.setFillColor(sf::Color::White);
    sf::FloatRect hb = helpButtonText.getLocalBounds();
    helpButtonText.setOrigin(hb.left + hb.width / 2.f,
                             hb.top + hb.height / 2.f);
    helpButtonText.setPosition(
        helpButton.getPosition().x + helpButton.getSize().x / 2.f,
        helpButton.getPosition().y + helpButton.getSize().y / 2.f - 2.f);
    window.draw(helpButtonText);

    vecViz.reflow(static_cast<float>(window.getSize().x));
    listViz.reflow(static_cast<float>(window.getSize().x));
    vecViz.draw(window);
    listViz.draw(window);

    drawCommandPanel(window, font, vecViz, listViz);
    sf::Text recInd(recorder.isRecording() ? "REC ON (G)" : "REC OFF (G)", font,
                    14);
    recInd.setFillColor(recorder.isRecording() ? sf::Color(255, 80, 80)
                                               : sf::Color(160, 160, 160));
    recInd.setPosition(15.f, 15.f);
    window.draw(recInd);

    sf::Text capInd(vecViz.isCaptureEnabled() ? "CAPTURA ON (F)"
                                              : "CAPTURA OFF (F)",
                    font, 14);
    capInd.setFillColor(vecViz.isCaptureEnabled() ? sf::Color(80, 200, 80)
                                                  : sf::Color(120, 120, 120));
    capInd.setPosition(15.f, 33.f);
    window.draw(capInd);

    bool showSeed = recorder.isRecording() || rng.hasSeed();
    if (showSeed) {
      unsigned int seedVal = rng.hasSeed() ? rng.seed() : recorder.seed();
      sf::Text seedText("SEED=" + std::to_string(seedVal) + " (G)", font, 14);
      seedText.setFillColor(sf::Color(200, 160, 40));
      seedText.setPosition(15.f, 51.f);
      window.draw(seedText);
    }

    if (timedReplayActive) {
      sf::Text replayText(std::string("Replay Temporal (K) t=") +
                              std::to_string(timedReplayClock) +
                              (timedReplayPaused ? " [PAUSADO]" : "") +
                              " speed=" + std::to_string(timedReplaySpeed),
                          font, 14);
      replayText.setFillColor(sf::Color(150, 200, 255));
      float replayY = showSeed ? 69.f : 55.f;
      replayText.setPosition(15.f, replayY);
      window.draw(replayText);
    }

    if (exportingFrames || exportingVideo) {
      std::string baseLabel =
          "Exportando: " + std::string(exportingFrames ? "Frames" : "") +
          (exportingFrames && exportingVideo ? " & " : "") +
          std::string(exportingVideo ? "Video" : "");
      std::string detail;
      if (exportingVideo) {
        std::lock_guard<std::mutex> lk(videoProgressMutex);
        detail = videoProgressLine;
      }
      if (exportingFrames && framesProgressTotal > 0) {
        double pct =
            (double)framesProgressCurrent / (double)framesProgressTotal * 100.0;
        std::ostringstream oss;
        oss << "frames " << framesProgressCurrent << "/" << framesProgressTotal
            << " (" << std::fixed << std::setprecision(1) << pct << "%)";
        if (!detail.empty())
          detail += " | ";

        detail += oss.str();
      }

      float secs = exportClock.getElapsedTime().asSeconds();
      if (secs > 0.f) {
        std::ostringstream mt;
        if (exportingFrames && framesProgressCurrent > 0) {
          double fpsWrite = framesProgressCurrent / secs;
          mt << std::fixed << std::setprecision(1) << fpsWrite << " fps-save";
        }

        if (exportingVideo && !detail.empty()) {
          if (!mt.str().empty())
            detail += " | ";

          mt << std::fixed << std::setprecision(1) << (secs) << "s";
        }

        if (!mt.str().empty()) {
          if (!detail.empty())
            detail += " | ";

          detail += mt.str();
        }
      }

      sf::Text expText(baseLabel + (detail.empty() ? "" : (" | " + detail)),
                       font, 14);
      expText.setFillColor(sf::Color(255, 180, 80));
      float yBase = (recorder.isRecording() || rng.hasSeed())
                        ? (timedReplayActive ? 109.f : 89.f)
                        : (timedReplayActive ? 95.f : 75.f);
      expText.setPosition(15.f, yBase);
      window.draw(expText);

      float barStartY = yBase + 18.f;
      float barWidth = 320.f;
      float barHeight = 10.f;

      if (exportingFrames && framesProgressTotal > 0) {
        sf::RectangleShape bg(sf::Vector2f(barWidth, barHeight));
        bg.setPosition(15.f, barStartY);
        bg.setFillColor(sf::Color(60, 60, 60));
        window.draw(bg);

        float pctW = (float)framesProgressCurrent / (float)framesProgressTotal *
                     barWidth;
        sf::RectangleShape fg(sf::Vector2f(pctW, barHeight));
        fg.setPosition(15.f, barStartY);
        fg.setFillColor(sf::Color(100, 200, 100));
        window.draw(fg);
        barStartY += 14.f;
      }

      if (exportingVideo) {
        size_t posFrame = videoProgressLine.find("frame ");
        size_t slashPos = videoProgressLine.find('/');
        double cur = 0, tot = 0;

        if (posFrame != std::string::npos && slashPos != std::string::npos &&
            slashPos > posFrame) {
          try {
            cur = std::stod(videoProgressLine.substr(
                posFrame + 6, slashPos - (posFrame + 6)));
          } catch (...) {
          }

          size_t afterTot = videoProgressLine.find(' ', slashPos + 1);

          if (afterTot == std::string::npos)
            afterTot = videoProgressLine.size();

          try {
            tot = std::stod(videoProgressLine.substr(
                slashPos + 1, afterTot - (slashPos + 1)));
          } catch (...) {
          }
        }

        if (tot <= 0)
          tot = (double)vecViz.getCapturedFrameCount();

        if (tot <= 0)
          tot = (double)vecViz.getCaptureLimit();

        if (tot > 0) {
          sf::RectangleShape bg(sf::Vector2f(barWidth, barHeight));
          bg.setPosition(15.f, barStartY);
          bg.setFillColor(sf::Color(60, 60, 60));
          window.draw(bg);

          float pctW = (float)(cur / tot) * barWidth;
          sf::RectangleShape fg(sf::Vector2f(pctW, barHeight));
          fg.setPosition(15.f, barStartY);
          fg.setFillColor(sf::Color(200, 180, 100));
          window.draw(fg);
          barStartY += 14.f;
        }

        if (videoCancelRequested) {
          sf::Text cancelText("Cancelando...", font, 12);
          cancelText.setFillColor(sf::Color(255, 100, 100));
          cancelText.setPosition(15.f, barStartY);
          window.draw(cancelText);
        }
      }

      if (framesCancelRequested) {
        sf::Text cancelText("Cancelando frames...", font, 12);
        cancelText.setFillColor(sf::Color(255, 100, 100));
        cancelText.setPosition(15.f, barStartY);
        window.draw(cancelText);
      }
    }

    if (showLimitStatus) {
      sf::Text limText("Limite: " + std::to_string(vecViz.getCaptureLimit()) +
                           " frames (T)",
                       font, 14);
      limText.setFillColor(sf::Color(180, 255, 180));
      float limYBase = timedReplayActive ? (showSeed ? 89.f : 75.f)
                                         : (showSeed ? 69.f : 55.f);
      limText.setPosition(15.f, limYBase);
      window.draw(limText);
      showLimitStatus = false;
    }

    for (auto &s : subtitles)
      s.age += dt;
    subtitles.erase(std::remove_if(subtitles.begin(), subtitles.end(),
                                   [SUBTITLE_DURATION](const Subtitle &s) {
                                     return s.age > SUBTITLE_DURATION;
                                   }),
                    subtitles.end());
    float baseY = window.getSize().y - 30.f;
    sf::Text subT("", font, 16);
    for (int i = (int)subtitles.size() - 1, line = 0; i >= 0; --i, ++line) {
      const auto &s = subtitles[i];
      float alphaRatio = std::max(0.f, SUBTITLE_DURATION - s.age) /
                         SUBTITLE_DURATION; // fade out
      sf::Color c(230, 230, 230, static_cast<sf::Uint8>(alphaRatio * 255));
      subT.setFillColor(c);
      subT.setString(s.text);
      sf::FloatRect b = subT.getLocalBounds();
      subT.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
      subT.setPosition(window.getSize().x / 2.f, baseY - line * 22.f);
      window.draw(subT);
    }

    vecViz.captureFrame(window);
    listViz.captureFrame(window);

    window.display();

    if (showHelpWindow && helpWindow && helpWindow->isOpen()) {
      sf::Event hevent;
      while (helpWindow->pollEvent(hevent)) {
        if (hevent.type == sf::Event::Closed) {
          helpWindow->close();
          showHelpWindow = false;
        }
        if (hevent.type == sf::Event::KeyPressed &&
            hevent.key.code == sf::Keyboard::Escape) {
          helpWindow->close();
          showHelpWindow = false;
        }
      }
      helpWindow->clear(sf::Color(25, 25, 25));
      sf::Text header("Lista de Comandos", font, 24);
      header.setFillColor(sf::Color(240, 240, 240));
      header.setPosition(20.f, 15.f);
      helpWindow->draw(header);

      float y = 60.f;
      sf::Text line("", font, 18);
      for (const auto &p : COMMAND_HELP) {
        line.setString("[" + p.first + "] " + p.second);
        line.setFillColor(sf::Color(200, 200, 200));
        line.setPosition(30.f, y);
        helpWindow->draw(line);
        y += 26.f;
      }

      sf::Text footer("ESC para fechar", font, 14);
      footer.setFillColor(sf::Color(180, 180, 180));
      footer.setPosition(30.f, helpWindow->getSize().y - 45.f);
      helpWindow->draw(footer);
      helpWindow->display();
    }
  }

  return 0;
}
