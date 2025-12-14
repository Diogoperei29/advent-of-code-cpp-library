#include "chrym.hpp"

using namespace chrym;

struct Solutions {
    std::vector<std::string> lines;

    explicit Solutions(std::string_view path) {
        lines = read_lines(path, false);
    }

    // Example placeholders to customize per day
    long long part1() {
        long long acc = 0;
        for (auto& line : lines) {
            for (char c : line) if (is_digit(c)) acc += (c - '0');
        }
        return acc;
    }

    long long part2() {
        long long acc = 0;
        for (auto& line : lines) {
            acc += static_cast<long long>(line.size());
        }
        return acc;
    }
};

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string input_path = (argc > 1) ? argv[1] : "input.txt";

    Solutions sol(input_path);

    auto [t1, ans1] = time_call([&]() { return sol.part1(); });
    auto [t2, ans2] = time_call([&]() { return sol.part2(); });

    std::cout << "Part 1: " << ans1 << " (" << t1 << " ms)\n";
    std::cout << "Part 2: " << ans2 << " (" << t2 << " ms)\n";

    int bench_iters = 50;
    double avg1 = benchmark([&]() { (void)sol.part1(); }, bench_iters);
    double avg2 = benchmark([&]() { (void)sol.part2(); }, bench_iters);

    std::cout << "Benchmark over " << bench_iters << " iters - Part 1: "
              << avg1 << " ms, Part 2: " << avg2 << " ms\n";

    return 0;
}
