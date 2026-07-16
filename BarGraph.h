#pragma once
#include <string>
#include <vector>

// Draws a simple ASCII bar graph in the terminal for any labeled set of
// values - used for the dashboard's income/tax history trends.
void drawBarGraph(const std::vector<std::string>& labels, const std::vector<double>& values, int maxBarWidth = 40);
