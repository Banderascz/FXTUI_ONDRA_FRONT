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

using json = nlohmann::json;

std::string error_cod;
CURL *curl = curl_easy_init();
CURLcode res;

void action(std::string &username, std::string &colour, std::string &password)
{
    if (curl)
    {
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

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        res = curl_easy_perform(curl);
        // Pročištění curl
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        // Error kódy (pan Ruperspác neustále v tom dělá bordel :( )
        switch (res)
        {
        case 0:
            error_cod = "no errors";
            break;
        case 1:
            error_cod = "username too short at least 4 characters";
            break;
        case 2:
            error_cod = "username too long maximum is 85 characters";
            break;
        case 3:
            error_cod = "username alredy taken";
            break;
        case 4:
            error_cod = "invalid colour format";
            break;
        case 5:
            error_cod = "password is empty";
            break;
        default:
            error_cod = "unknown error";
        }
        /*std::cout << std::endl
                  << error_cod;*/
    }
}

int main()
{
    using namespace ftxui;

    // InputOption style_1 = InputOption::Default();

    /*InputOption style_2 = InputOption::Spacious();

    InputOption style_3 = InputOption::Spacious();
    style_3.transform = [](InputState state)
    {
        state.element |= borderEmpty;

        if (state.is_placeholder)
        {
            state.element |= dim;
        }

        if (state.focused)
        {
            state.element |= borderDouble;
            state.element |= bgcolor(Color::White);
            state.element |= color(Color::Black);
        }
        else if (state.hovered)
        {
            state.element |= borderRounded;
            state.element |= bgcolor(LinearGradient(90, Color::Blue, Color::Red));
            state.element |= color(Color::White);
        }
        else
        {
            state.element |= border;
            state.element |= bgcolor(LinearGradient(0, Color::Blue, Color::Red));
            state.element |= color(Color::White);
        }

        return state.element;
    };

    InputOption style_4 = InputOption::Spacious();
    style_4.transform = [](InputState state)
    {
        state.element = hbox({
            text("Theorem") | center | borderEmpty | bgcolor(Color::Red),
            separatorEmpty(),
            separator() | color(Color::White),
            separatorEmpty(),
            std::move(state.element),
        });

        state.element |= borderEmpty;
        if (state.is_placeholder)
        {
            state.element |= dim;
        }

        if (state.focused)
        {
            state.element |= bgcolor(Color::Black);
        }
        else
        {
            state.element |= bgcolor(Color::Blue);
        }

        if (state.hovered)
        {
            state.element |= bgcolor(Color::GrayDark);
        }

        return vbox({state.element, separatorEmpty()});
    };*/

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
                 { return text("Errors: " + error_cod); });
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