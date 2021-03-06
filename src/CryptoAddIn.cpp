/*
 *  Modern Native AddIn
 *  Copyright (C) 2018  Infactum
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include "CryptoAddIn.h"
#include "MyCrypto.h"

std::string CryptoAddIn::extensionName() {
    return "MyCrypto";
}

CryptoAddIn::CryptoAddIn() {
    // Universal property. Could store any supported by native api type.
    crypto_property = std::make_shared<variant_t>();
    AddProperty(L"CryptoProperty", L"ОбразецСвойства", crypto_property);

    // Full featured property registration example
    AddProperty(L"Version", L"ВерсияКомпоненты", [&]() {
        auto s = std::string(Version);
        return std::make_shared<variant_t>(std::move(s));
    });

    // Method registration.
    // Lambdas as method handlers are not supported.
    AddMethod(L"Add", L"Сложить", this, &CryptoAddIn::add);
    AddMethod(L"Message", L"Сообщить", this, &CryptoAddIn::message);
    AddMethod(L"CurrentDate", L"ТекущаяДата", this, &CryptoAddIn::currentDate);
    AddMethod(L"Assign", L"Присвоить", this, &CryptoAddIn::assign);
    AddMethod(L"CryptoPropertyValue", L"ЗначениеСвойстваОбразца", this, &CryptoAddIn::cryptoPropertyValue);

    AddMethod(L"Test", L"Тест", this, &CryptoAddIn::test);
    AddMethod(L"CryptGenRandom", L"ПолучитьКриптоДанные", this, &CryptoAddIn::cryptGenRandom);

    // Method registration with default arguments
    //
    // Notice that if variant_t would be non-copy you can't use initializer list.
    // Proper way to register def args would be then:
    //        std::map<long, variant_t> def_args;
    //        def_args.insert({0, 5});
    //        AddMethod(u"Sleep", u"Ожидать", this, &CryptoAddIn::sleep, std::move(def_args));
    //
    AddMethod(L"Sleep", L"Ожидать", this, &CryptoAddIn::sleep, {{0, 5}});

}

// Sample of addition method. Support both integer and string params.
// Every exceptions derived from std::exceptions are handled by components API
variant_t CryptoAddIn::add(const variant_t &a, const variant_t &b) {
    if (std::holds_alternative<int32_t>(a) && std::holds_alternative<int32_t>(b)) {
        return std::get<int32_t>(a) + std::get<int32_t>(b);
    } else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
        return std::string{std::get<std::string>(a) + std::get<std::string>(b)};
    } else {
        throw std::runtime_error(u8"Неподдерживаемые типы данных");
    }
}

void CryptoAddIn::message(const variant_t &msg) {
    std::visit(overloaded{
            [&](const std::string &v) { AddError(ADDIN_E_INFO, extensionName(), v, false); },
            [&](const int32_t &v) {
                AddError(ADDIN_E_INFO, extensionName(), std::to_string(static_cast<int>(v)), false);
            },
            [&](const double &v) { AddError(ADDIN_E_INFO, extensionName(), std::to_string(v), false); },
            [&](const bool &v) {
                AddError(ADDIN_E_INFO, extensionName(), std::string(v ? u8"Истина" : u8"Ложь"), false);
            },
            [&](const std::tm &v) {
                std::ostringstream oss;
                oss.imbue(std::locale("ru_RU.utf8"));
                oss << std::put_time(&v, "%c");
                AddError(ADDIN_E_INFO, extensionName(), oss.str(), false);
            },
            [&](const std::vector<char> &v) {},
            [&](const std::monostate &) {}
    }, msg);
}

void CryptoAddIn::sleep(const variant_t &delay) {
    using namespace std;
    // It safe to get any type from variant.
    // Exceptions are handled by component API.
    this_thread::sleep_for(chrono::seconds(get<int32_t>(delay)));
}

// Out params support option must be enabled for this to work
void CryptoAddIn::assign(variant_t &out) {
    out = true;
}

// Despite that you can return property value through method this is not recommended
// due to unwanted data copying
variant_t CryptoAddIn::cryptoPropertyValue() {
    return *crypto_property;
}

variant_t CryptoAddIn::currentDate() {
    using namespace std;
    tm current{};
    time_t t = time(nullptr);
#ifdef _WINDOWS
    localtime_s(&current, &t);
#else
    localtime_r(&t, &current);
#endif
    return current;
}

variant_t CryptoAddIn::test() {
    return std::string("Тестовая строка");
}

variant_t CryptoAddIn::cryptGenRandom() {
    MyCrypto crypto = MyCrypto(_T("MyKeyContainer"));
    const DWORD sizeofData = 16;
    BYTE data[sizeofData];
    if (crypto.GenerateRandom(sizeofData, data)) {
        //std::vector<wchar_t> text(std::begin(data), std::end(data));
        return std::vector<char>(std::begin(data), std::end(data));
    }
    return std::string("Что-то пошло не так...");
}
