#pragma once

#include <nlohmann/json.hpp>
#include<lib/Backend.h>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/dom/elements.hpp>

const int kHoursInDay = 24;
const int kDateStringLength = 10;
const int kNightHour = 0;
const int kMorningHour = 6;
const int kAfternoonHour = 12;
const int kEveningHour = 18;
const int kDaysPerPage = 3;

ftxui::Element CreateTimeBox(nlohmann::json weather_json, int day_number, int time);

ftxui::Element CreateDayBox(nlohmann::json weather_json, int day_number);

ftxui::Element CreatePage(nlohmann::json weather_json, int days_amount, int current_page, int pages_amount);

void DisplayWeather(nlohmann::json config);