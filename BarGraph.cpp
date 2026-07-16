#include "BarGraph.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
using namespace std;

void drawBarGraph(const vector<string>& labels, const vector<double>& values, int maxBarWidth) {
    if (values.empty()) return;

    double maxValue = 0;
    for (double v : values) maxValue = max(maxValue, v);
    if (maxValue <= 0) maxValue = 1; // avoid divide-by-zero if everything is 0

    for (size_t i = 0; i < values.size(); i++) {
        int barLength = (int)((values[i] / maxValue) * maxBarWidth);

        // Fix: if one entry is far larger than the rest (e.g. a mistyped
        // huge income), smaller-but-still-positive values scale down to 0
        // and their bar silently disappears. Give any positive value at
        // least 1 character so it's still visible on the graph.
        if (barLength == 0 && values[i] > 0) {
            barLength = 1;
        }

        cout << setw(15) << left << labels[i] << " | ";
        for (int j = 0; j < barLength; j++) cout << "#";
        cout << "  " << fixed << setprecision(2) << values[i] << "\n";
    }
    cout << "\n";
}
