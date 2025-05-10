#pragma once

#include<cpr/cpr.h>
#include <nlohmann/json.hpp>
#include<iostream>

nlohmann::json GetBaseInfo(std::string name);

nlohmann::json GetWeather(double latitude, double longitude, int forecast_days);

std::vector<nlohmann::json> GetWeathers(nlohmann::json config);