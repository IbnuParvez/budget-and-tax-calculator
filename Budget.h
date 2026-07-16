#pragma once

// Splits post-tax income using percentages the user chooses themselves
// (needsPct + personalPct + savingsPct should sum to 100).
void showBudgetAllocation(double remainingIncome, double needsPct, double personalPct, double savingsPct);
