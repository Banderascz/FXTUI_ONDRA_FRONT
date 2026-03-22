// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/dom/linear_gradient.hpp> // for LinearGradient
#include <ftxui/screen/color.hpp>        // for Color, Color::White, Color::Red, Color::Blue, Color::Black, Color::GrayDark, ftxui
#include <functional>                    // for function
#include <string>                        // for allocator, string
#include <utility>                       // for move

#include "ftxui/component/app.hpp"               // for App
#include "ftxui/component/component.hpp"         // for Input, Horizontal, Vertical, operator|
#include "ftxui/component/component_base.hpp"    // for Component
#include "ftxui/component/component_options.hpp" // for InputState, InputOption
#include "ftxui/dom/elements.hpp"                // for operator|=, Element, bgcolor, operator|, separatorEmpty, color, borderEmpty, separator, text, center, dim, hbox, vbox, border, borderDouble, borderRounded
#include "ftxui/component/captured_mouse.hpp"    // for ftxui

#include <curl/curl.h> // for CURL

#include <nlohmann/json.hpp> // for JSON

#include <iostream>
#include <cstring>
#include <ostream>
#include <sstream>
#include <iomanip>

#include <openssl/sha.h>
std::string sha256(const std::string &str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

using json = nlohmann::json;

std::string readBuffer, Error_from_buffer;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void action(std::string &username, std::string &colour, std::string &password)
{
    CURL *curl = curl_easy_init();
    CURLcode res;
    password = sha256(password);
    if (curl)
    {
        char errbuf[CURL_ERROR_SIZE];
        // ŽSON Skládání
        json j;
        j["username"] = username;
        j["colour"] = colour;
        j["password"] = password;

        std::string json_string = j.dump();
        // Nastavení URL
        curl_easy_setopt(curl, CURLOPT_URL,
                         "https://ondra.rapspace.com/dev-versions/sqlchad/signin/signinback.php");
        // Předání ŽSONU
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_string.c_str());
        // Setup pro error kody
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
        /*Přesměrování write callbacku*/
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        /* set the error buffer as empty before performing a request */
        errbuf[0] = 0;
        // headery
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        // spusteni CURL
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            size_t len = strlen(errbuf);
            fprintf(stderr, "\nlibcurl: (%d) ", res);
            if (len)
                fprintf(stderr, "%s%s", errbuf,
                        ((errbuf[len - 1] != '\n') ? "\n" : ""));
            else
                fprintf(stderr, "%s\n", curl_easy_strerror(res));
        }

        json rB;

        std::stringstream(readBuffer) >> rB;

        int errorcod;
        // vypis JSONU
        for (auto &[key, val] : rB.items())
        {
            if (key == "status")
            {
                errorcod = val;
                // std::cout << val << std::endl;
            }
        }
        // std::cout << errorcod << std::endl;
        //  Error kódy (pan Ruperspác neustále v tom dělá bordel :(
        switch (errorcod)
        {
        case 0:
            Error_from_buffer = "Žádné chyby";
            break;
        case 1:
            Error_from_buffer = "Přihlašovací jméno je příliš krátké, minimum jsou 4 slova.";
            break;
        case 2:
            Error_from_buffer = "Přihlašovací jméno je příliš dlouhé, maximum je 85 slov.";
            break;
        case 3:
            Error_from_buffer = "Přihlašovací jméno už je zabrané.";
            break;
        case 4:
            Error_from_buffer = "Neplatný formát pro barvu.";
            break;
        case 5:
            Error_from_buffer = "Heslo je prázdné.";
            break;
        case 6:
            Error_from_buffer = "Heslo je příliš dlouhé, maximum je 85 slov.";
            break;
        default:
            Error_from_buffer = "Neznámá chyba.";
        }

        // Pročištění curl
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        rB.clear();
        j.clear();
    }
}

int main()
{
    using namespace ftxui;

    InputOption style_5 = InputOption::Spacious();
    style_5.transform = [](InputState state)
    {
        state.element = hbox({
            text("Zadejte: ") | center | borderRounded,
            separatorEmpty(),
            separator() | color(Color::Blue),
            separatorEmpty(),
            std::move(state.element),
        });

        state.element |= borderEmpty;
        if (state.is_placeholder)
        {
            state.element |= bold;
            state.element |= color(Color::White);
            state.element |= bgcolor(Color::Black);
        }

        if (state.focused)
        {
            state.element |= color(Color::Black);
            state.element |= bgcolor(Color::White);
        }
        else
        {
            state.element |= color(Color::White);
            state.element |= bgcolor(Color::Black);
        }

        if (state.hovered)
        {
            state.element |= color(Color::White);
            state.element |= bgcolor(Color::Black);
        }

        return vbox({
            state.element,
            separatorEmpty(),
        });
    };

    std::string username;
    std::string password;
    std::string color;
    auto action_acivate = [&]
    { action(username, color, password); };

    auto action_renderer =
        Renderer([&]
                 { return text("Errors: " + Error_from_buffer + "\n"); });
    auto generateUiFromStyle = [&](InputOption style)
    {
        return Container::Vertical({
                   Input(username, "Username", style),
                   Input(password, "Heslo", style),
                   Input(color, "Barva", style),

               }) |
               borderEmpty;
    };

    auto ui = Container::Horizontal({
        // generateUiFromStyle(style_1),
        generateUiFromStyle(style_5),
        Renderer([]
                 { return separator(); }),
        action_renderer,
        Renderer([]
                 { return separator(); }),

        Container::Vertical({
            Button("POTVRDIT", action_acivate, ButtonOption::Ascii()),
        }),
    });

    auto screen = App::TerminalOutput();
    screen.Loop(ui);
}