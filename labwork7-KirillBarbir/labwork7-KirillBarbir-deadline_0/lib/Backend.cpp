#include "Backend.h"

nlohmann::json GetBaseInfo(std::string name) {
    cpr::Response r = cpr::Get(cpr::Url{"https://api.api-ninjas.com/v1/city?name=" + name + "&limit=30"},
                               cpr::Header{{"X-Api-Key", "/wjwzfoLDmGQx8F7mFTJZg==n0nSkxIGxPYZ8Dvb"}});
    nlohmann::json js1 = nlohmann::json::parse(r.text);
    return js1;
}

nlohmann::json GetWeather(double latitude, double longitude, int forecast_days) {
    std::string s1;

    cpr::Response r = cpr::Get(cpr::Url{
        "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(latitude) + "&longitude="
            + std::to_string(longitude) + "&hourly="
            + "temperature_2m,relative_humidity_2m,precipitation_probability,precipitation,weather_code,cloud_cover,wind_speed_10m,wind_direction_10m&forecast_days="
            + std::to_string(forecast_days)});

    nlohmann::json js1 = nlohmann::json::parse(r.text);
    return js1;
}

std::vector<nlohmann::json> GetWeathers(nlohmann::json config) {
    std::vector<nlohmann::json> out;
    nlohmann::json apininjas_info;

    for (int i = 0; i < config["city"].size(); ++i) {
        apininjas_info = GetBaseInfo(config["city"][0]);
        out.push_back(GetWeather(apininjas_info[i]["latitude"], apininjas_info[i]["longitude"], 16));
    }

    return out;
}