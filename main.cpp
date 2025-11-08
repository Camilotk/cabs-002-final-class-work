#include <SFML/Graphics.hpp>
#include "VectorVisualizer.h"
#include "LinkedListVisualizer.h"
#include "Command.h"
#include <iostream>
#include <ctime>
#include <format>
#include <string>
#include <vector>
#include <deque>
#include <functional>

void drawCommandPanel(sf::RenderWindow& window, sf::Font& font,
                      const VectorVisualizer& vecViz, const LinkedListVisualizer& listViz) {

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

    const auto& vecQueue = vecViz.getOperationQueue();
    const auto& listQueue = listViz.getOperationQueue();

    std::vector<std::string> allCommands;
    for(const auto& cmd : vecQueue) allCommands.push_back(cmd.description);
    for(const auto& cmd : listQueue) allCommands.push_back(cmd.description);

    sf::Text commandText("", font, 16);
    commandText.setFillColor(sf::Color(220, 220, 220));
    float currentY = 60.f;

    for(const auto& desc : allCommands) {
        commandText.setString(desc);
        commandText.setPosition(panelX + 15, currentY);
        window.draw(commandText);
        currentY += 25.f;
    }
}


int main() {
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(1200, 600), "Visualizador Animado de Estruturas de Dados");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Erro ao carregar fonte\n";
        return -1;
    }

    sf::Text titleText("Visualizador de Estruturas de Dados", font, 24);
    sf::Text instructionsText; // Declarado aqui, configurado depois

    VectorVisualizer vecViz(font, {50.f, 150.f});
    LinkedListVisualizer listViz(font, {50.f, 400.f});

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::I) vecViz.insert(rand() % 100, 2);
                else if (event.key.code == sf::Keyboard::R) vecViz.remove(2);
                else if (event.key.code == sf::Keyboard::A) listViz.push_front(rand() % 100);
                else if (event.key.code == sf::Keyboard::D) listViz.pop_front();
            }
        }

        vecViz.update(dt);
        listViz.update(dt);

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
        instructionsText.setOrigin(instructionsBounds.left + instructionsBounds.width / 2.0f, instructionsBounds.top + instructionsBounds.height / 2.0f);
        instructionsText.setPosition(window.getSize().x / 2.0f, 70.f);

        window.draw(titleText);
        window.draw(instructionsText);

        vecViz.draw(window);
        listViz.draw(window);

        drawCommandPanel(window, font, vecViz, listViz);

        window.display();
    }

    return 0;
}
