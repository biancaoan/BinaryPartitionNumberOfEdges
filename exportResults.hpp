#ifndef EXPORT_RESULTS_HPP
#define EXPORT_RESULTS_HPP

#include <vector>
#include <string>

struct Output;

void writeResultsToJson(const std::vector<Output>& results, const std::string& filename);

#endif
