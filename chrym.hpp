#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace chrym {

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using usize = std::size_t;

using f32 = float;
using f64 = double;

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

// ------------------------
// Small generic utilities
// ------------------------

// Clamp value v to [lo, hi]
template <class T>
constexpr T clamp(T v, T lo, T hi) {
	return v < lo ? lo : (v > hi ? hi : v);
}

// Sign function: -1, 0, or 1
template <class T>
constexpr int sgn(T v) {
	return (T(0) < v) - (v < T(0));
}

// Check if v is in [lo, hi]
template <class T>
constexpr bool between_inclusive(T v, T lo, T hi) {
	return lo <= v && v <= hi;
}

// Check if v is in (lo, hi)
template <class T>
constexpr bool between_exclusive(T v, T lo, T hi) {
	return lo < v && v < hi;
}

// Check if container c contains value (uses .find() or linear search)
template <class Container, class T>
bool contains(const Container& c, const T& value) {
	if constexpr (requires { c.find(value); }) {
		return c.find(value) != c.end();
	} else {
		return std::find(c.begin(), c.end(), value) != c.end();
	}
}

// ------------------------
// String utilities
// ------------------------

// Convert string_view to string
inline std::string to_string(std::string_view sv) { return std::string(sv); }

// Trim whitespace from left side (in-place)
inline std::string& ltrim_inplace(std::string& s) {
	auto it = std::find_if_not(s.begin(), s.end(), [](unsigned char ch) { return std::isspace(ch); });
	s.erase(s.begin(), it);
	return s;
}

// Trim whitespace from right side (in-place)
inline std::string& rtrim_inplace(std::string& s) {
	auto it = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char ch) { return std::isspace(ch); });
	s.erase(it.base(), s.end());
	return s;
}

// Trim whitespace from both sides (in-place)
inline std::string& trim_inplace(std::string& s) {
	return ltrim_inplace(rtrim_inplace(s));
}

// Trim whitespace from left side (copy)
inline std::string ltrim(std::string_view sv) {
	std::string s(sv);
	return ltrim_inplace(s);
}

// Trim whitespace from right side (copy)
inline std::string rtrim(std::string_view sv) {
	std::string s(sv);
	return rtrim_inplace(s);
}

// Trim whitespace from both sides (copy)
inline std::string trim(std::string_view sv) {
	std::string s(sv);
	return trim_inplace(s);
}

// Convert string to lowercase (copy)
inline std::string to_lower_copy(std::string_view sv) {
	std::string s(sv);
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
	return s;
}

// Convert string to uppercase (copy)
inline std::string to_upper_copy(std::string_view sv) {
	std::string s(sv);
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
	return s;
}

// Check if string starts with prefix
inline bool starts_with(std::string_view s, std::string_view prefix) {
	return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
}

// Check if string ends with suffix
inline bool ends_with(std::string_view s, std::string_view suffix) {
	return s.size() >= suffix.size() && s.substr(s.size() - suffix.size()) == suffix;
}

// Case-insensitive string equality
inline bool icase_equal(std::string_view a, std::string_view b) {
	if (a.size() != b.size()) return false;
	for (usize i = 0; i < a.size(); ++i) {
		if (std::tolower(static_cast<unsigned char>(a[i])) !=
			std::tolower(static_cast<unsigned char>(b[i]))) return false;
	}
	return true;
}

// Split string by single delimiter character
inline std::vector<std::string> split(std::string_view s, char delim, bool skip_empty = false) {
	std::vector<std::string> res;
	std::string cur;
	for (char c : s) {
		if (c == delim) {
			if (!cur.empty() || !skip_empty) res.push_back(std::move(cur));
			cur.clear();
		} else {
			cur.push_back(c);
		}
	}
	if (!cur.empty() || !skip_empty) res.push_back(std::move(cur));
	return res;
}

// Split string by any character in delims
inline std::vector<std::string> split_any(std::string_view s, std::string_view delims, bool skip_empty = false) {
	std::vector<std::string> res;
	std::string cur;
	auto is_delim = [delims](char c) { return delims.find(c) != std::string_view::npos; };
	for (char c : s) {
		if (is_delim(c)) {
			if (!cur.empty() || !skip_empty) res.push_back(std::move(cur));
			cur.clear();
		} else {
			cur.push_back(c);
		}
	}
	if (!cur.empty() || !skip_empty) res.push_back(std::move(cur));
	return res;
}

// Split string into lines (handles \r\n)
inline std::vector<std::string> split_lines(std::string_view s, bool skip_empty = false) {
	std::vector<std::string> res;
	std::string cur;
	for (char c : s) {
		if (c == '\n') {
			if (!cur.empty() || !skip_empty) {
				if (!cur.empty() && cur.back() == '\r') cur.pop_back();
				res.push_back(std::move(cur));
			}
			cur.clear();
		} else {
			cur.push_back(c);
		}
	}
	if (!cur.empty() || !skip_empty) {
		if (!cur.empty() && cur.back() == '\r') cur.pop_back();
		res.push_back(std::move(cur));
	}
	return res;
}

// Join vector of strings with delimiter
inline std::string join(const std::vector<std::string>& parts, std::string_view delim) {
	if (parts.empty()) return {};
	std::string out;
	usize total = (parts.size() - 1) * delim.size();
	for (auto& p : parts) total += p.size();
	out.reserve(total);
	for (usize i = 0; i < parts.size(); ++i) {
		if (i) out.append(delim);
		out.append(parts[i]);
	}
	return out;
}

// Parse comma-separated integers
inline std::vector<int> to_ints(std::string_view s, char delim = ',') {
	auto parts = split(s, delim, true);
	std::vector<int> out;
	out.reserve(parts.size());
	for (auto& p : parts) {
		out.push_back(std::stoi(p));
	}
	return out;
}

// Parse comma-separated long longs
inline std::vector<long long> to_lls(std::string_view s, char delim = ',') {
	auto parts = split(s, delim, true);
	std::vector<long long> out;
	out.reserve(parts.size());
	for (auto& p : parts) {
		out.push_back(std::stoll(p));
	}
	return out;
}

// Fast integer parsing using from_chars
template <class Int>
std::optional<Int> parse_int(std::string_view s, int base = 10) {
	Int value{};
	auto first = s.data();
	auto last = s.data() + s.size();
	auto [ptr, ec] = std::from_chars(first, last, value, base);
	if (ec != std::errc{} || ptr != last) return std::nullopt;
	return value;
}

// Repeat string n times
inline std::string repeat(std::string_view s, usize n) {
	std::string out;
	out.reserve(s.size() * n);
	for (usize i = 0; i < n; ++i) out.append(s);
	return out;
}

// Reverse string (copy)
inline std::string reverse_copy(std::string_view s) {
	std::string out(s.rbegin(), s.rend());
	return out;
}

// ------------------------
// File and input helpers
// ------------------------

// Read entire file into string
inline std::string read_file(std::string_view path) {
	std::ifstream in{std::string(path)};
	if (!in) throw std::runtime_error("cannot open file");
	std::ostringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

// Read file and split into lines
inline std::vector<std::string> read_lines(std::string_view path, bool skip_empty = false) {
	return split_lines(read_file(path), skip_empty);
}

// ------------------------
// Range / vector helpers
// ------------------------

// Sum of all elements
template <class T>
inline T sum(const std::vector<T>& v) {
	return std::accumulate(v.begin(), v.end(), T{});
}

// Sum of all elements as long long
template <class T>
inline long long sum_ll(const std::vector<T>& v) {
	return std::accumulate(v.begin(), v.end(), 0ll);
}

// Product of all elements
template <class T>
inline T product(const std::vector<T>& v) {
	return std::accumulate(v.begin(), v.end(), T{1}, std::multiplies<>());
}

// Minimum element value
template <class T>
inline T min_value(const std::vector<T>& v) {
	return *std::min_element(v.begin(), v.end());
}

// Maximum element value
template <class T>
inline T max_value(const std::vector<T>& v) {
	return *std::max_element(v.begin(), v.end());
}

// Sort vector in ascending order (in-place)
template <class T>
inline void sort_asc(std::vector<T>& v) {
	std::sort(v.begin(), v.end());
}

// Sort vector in descending order (in-place)
template <class T>
inline void sort_desc(std::vector<T>& v) {
	std::sort(v.begin(), v.end(), std::greater<>());
}

// Return sorted copy of vector
template <class T>
inline std::vector<T> sorted_copy(std::vector<T> v) {
	sort_asc(v);
	return v;
}

// Return sorted vector with duplicates removed
template <class T>
inline std::vector<T> unique_sorted_copy(std::vector<T> v) {
	sort_asc(v);
	v.erase(std::unique(v.begin(), v.end()), v.end());
	return v;
}

// Remove consecutive duplicates (in-place, assumes sorted)
template <class T>
inline void unique_inplace(std::vector<T>& v) {
	v.erase(std::unique(v.begin(), v.end()), v.end());
}

// Compute prefix sums
template <class T>
inline std::vector<T> prefix_sums(const std::vector<T>& v) {
	std::vector<T> out(v.size());
	std::partial_sum(v.begin(), v.end(), out.begin());
	return out;
}

// ------------------------
// Math helpers
// ------------------------

// Greatest common divisor
template <class T>
constexpr T gcd(T a, T b) {
	while (b != 0) {
		T t = a % b;
		a = b;
		b = t;
	}
	return a < 0 ? -a : a;
}

// Least common multiple
template <class T>
constexpr T lcm(T a, T b) {
	if (a == 0 || b == 0) return 0;
	return (a / gcd(a, b)) * b;
}

// Absolute difference |a - b|
template <class T>
constexpr T absdiff(T a, T b) {
	return a > b ? a - b : b - a;
}

// Manhattan distance in 2D
template <class T>
constexpr T manhattan_2d(T x1, T y1, T x2, T y2) {
	return absdiff(x1, x2) + absdiff(y1, y2);
}

// Check if x is a power of 2
template <class T>
constexpr bool is_power_of_two(T x) {
	if (x <= 0) return false;
	return (x & (x - 1)) == 0;
}

// Check if character is digit 0-9
inline bool is_digit(char c) { return c >= '0' && c <= '9'; }
// Check if character is alphabetic
inline bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
// Check if character is alphanumeric
inline bool is_alnum(char c) { return is_digit(c) || is_alpha(c); }

// ------------------------
// Grid helpers
// ------------------------

struct Point2i {
	int x{};
	int y{};
	auto operator<=>(const Point2i&) const = default;
};

struct PointHash {
	std::size_t operator()(const Point2i& p) const noexcept {
		std::size_t h1 = std::hash<int>{}(p.x);
		std::size_t h2 = std::hash<int>{}(p.y);
		return h1 * 1315423911u + h2;
	}
};

inline constexpr std::array<Point2i, 4> DIR4{{{1,0},{-1,0},{0,1},{0,-1}}};
inline constexpr std::array<Point2i, 8> DIR8{{{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}}};

// Check if (x, y) is within grid bounds
template <class Grid>
bool in_bounds(const Grid& g, int x, int y) {
	return y >= 0 && y < static_cast<int>(g.size()) && x >= 0 && x < static_cast<int>(g[y].size());
}

// Access grid element at point (non-const)
template <class Grid>
auto& at(Grid& g, const Point2i& p) { return g[p.y][p.x]; }

// Access grid element at point (const)
template <class Grid>
const auto& at(const Grid& g, const Point2i& p) { return g[p.y][p.x]; }

template <class T>
using Grid2D = std::vector<std::vector<T>>;

// Parse lines into 2D char grid
inline Grid2D<char> parse_char_grid(const std::vector<std::string>& lines) {
	Grid2D<char> g;
	g.reserve(lines.size());
	for (auto& line : lines) {
		g.emplace_back(line.begin(), line.end());
	}
	return g;
}

// Parse lines into 2D grid of single-digit integers
template <class T>
Grid2D<T> parse_int_grid(const std::vector<std::string>& lines) {
	Grid2D<T> g;
	g.reserve(lines.size());
	for (auto& line : lines) {
		std::vector<T> row;
		row.reserve(line.size());
		for (char c : line) {
			if (is_digit(c)) row.push_back(static_cast<T>(c - '0'));
		}
		g.push_back(std::move(row));
	}
	return g;
}

// BFS on grid returning distance matrix (-1 = unreachable)
template <class Grid, class Pred>
std::vector<std::vector<int>> bfs_dist_grid(const Grid& g, Point2i start, Pred passable) {
	int H = static_cast<int>(g.size());
	int W = H ? static_cast<int>(g[0].size()) : 0;
	std::vector<std::vector<int>> dist(H, std::vector<int>(W, -1));
	if (!in_bounds(g, start.x, start.y)) return dist;
	std::queue<Point2i> q;
	dist[start.y][start.x] = 0;
	q.push(start);
	while (!q.empty()) {
		auto p = q.front(); q.pop();
		int d = dist[p.y][p.x];
		for (auto dir : DIR4) {
			Point2i np{p.x + dir.x, p.y + dir.y};
			if (!in_bounds(g, np.x, np.y)) continue;
			if (dist[np.y][np.x] != -1) continue;
			if (!passable(at(g, np))) continue;
			dist[np.y][np.x] = d + 1;
			q.push(np);
		}
	}
	return dist;
}

// ------------------------
// Graph helpers
// ------------------------

// Adjacency list representation
using AdjList = std::vector<std::vector<int>>;

// Create empty adjacency list with n nodes
inline AdjList make_adj_list(int n) { return AdjList(n); }

// Add undirected edge between u and v
inline void add_undirected_edge(AdjList& g, int u, int v) {
	g[u].push_back(v);
	g[v].push_back(u);
}

// Add directed edge from u to v
inline void add_directed_edge(AdjList& g, int u, int v) {
	g[u].push_back(v);
}

// BFS from src returning distance to each node (-1 = unreachable)
inline std::vector<int> bfs_dist(const AdjList& g, int src) {
	std::vector<int> dist(g.size(), -1);
	std::queue<int> q;
	dist[src] = 0;
	q.push(src);
	while (!q.empty()) {
		int v = q.front(); q.pop();
		int d = dist[v];
		for (int to : g[v]) if (dist[to] == -1) {
			dist[to] = d + 1;
			q.push(to);
		}
	}
	return dist;
}

// ------------------------
// Timing / benchmarking
// ------------------------

// Simple timer using steady_clock
struct Timer {
	TimePoint start{Clock::now()};
	void reset() { start = Clock::now(); }
	[[nodiscard]] double ms() const {
		auto end = Clock::now();
		return std::chrono::duration<double, std::milli>(end - start).count();
	}
};

// Time a function call and return {elapsed_ms, result}
template <class F, class... Args>
auto time_call(F&& f, Args&&... args) {
	Timer t;
	if constexpr (std::is_void_v<std::invoke_result_t<F, Args...>>) {
		std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
		double elapsed = t.ms();
		return std::pair{elapsed, 0};
	} else {
		auto result = std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
		double elapsed = t.ms();
		return std::pair{elapsed, result};
	}
}

// Run function multiple times and return average ms per call
template <class F, class... Args>
double benchmark(F&& f, int iterations, Args&&... args) {
	Timer t;
	for (int i = 0; i < iterations; ++i) {
		std::invoke(f, args...);
	}
	return t.ms() / iterations;
}

// ------------------------
// Random helpers (for testing)
// ------------------------

// Get global random number generator
inline std::mt19937& rng() {
	static std::mt19937 gen(std::random_device{}());
	return gen;
}

// Generate random integer in [lo, hi]
template <class Int>
Int rand_int(Int lo, Int hi) {
	std::uniform_int_distribution<Int> dist(lo, hi);
	return dist(rng());
}

// Shuffle vector randomly
template <class T>
void shuffle(std::vector<T>& v) {
	std::shuffle(v.begin(), v.end(), rng());
}

} // namespace chrym

