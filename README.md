# Advent of Code C++ Utility Library

Single-header C++20 helper library (`chrym.hpp`) plus a small runner (`main.cpp`) tuned for Advent of Code style problems.

## Library overview (`namespace chrym`)

- **Numeric aliases**: `i32`, `i64`, `u32`, `u64`, `usize`, `f32`, `f64`.
- **Generic helpers**: `clamp`, `sgn`, `between_inclusive`, `between_exclusive`, `contains`.
- **Strings**
	- `trim`, `ltrim`, `rtrim` (+ `_inplace` variants).
	- `to_lower_copy`, `to_upper_copy`.
	- `starts_with`, `ends_with`, `icase_equal`.
	- `split`, `split_any`, `split_lines` (optionally skipping empty tokens).
	- `join` (vector of strings with delimiter).
	- `to_ints`, `to_lls` (split + `stoi`/`stoll`).
	- `parse_int<T>` (fast `from_chars`-based integer parse).
	- `repeat` (repeat string N times), `reverse_copy`.

- **File / input**
	- `read_file(path) -> std::string` (whole file).
	- `read_lines(path, skip_empty=false) -> std::vector<std::string>`.

- **Ranges / vectors**
	- `sum`, `sum_ll`, `product`.
	- `min_value`, `max_value`.
	- `sort_asc`, `sort_desc`, `sorted_copy`, `unique_sorted_copy`, `unique_inplace`.
	- `prefix_sums`.

- **Math**
	- `gcd`, `lcm`, `absdiff`, `manhattan_2d`.
	- `is_power_of_two`, `is_digit`, `is_alpha`, `is_alnum`.

- **Grid**
	- `Point2i { int x, y; }`, `PointHash` (for unordered containers).
	- Direction arrays: `DIR4`, `DIR8`.
	- `Grid2D<T> = std::vector<std::vector<T>>`.
	- `in_bounds(grid, x, y)`, `at(grid, point)`.
	- `parse_char_grid(lines)`, `parse_int_grid<T>(lines)`.
	- `bfs_dist_grid(grid, start, passable)` → distance grid (`-1` unreachable).

- **Graph**
	- `AdjList = std::vector<std::vector<int>>`.
	- `make_adj_list(n)`, `add_undirected_edge`, `add_directed_edge`.
	- `bfs_dist(graph, src)` → distance per node.

- **Timing / benchmarking**
	- `Timer` (start on construction, `ms()` for elapsed milliseconds).
	- `time_call(f, args...)` → `{elapsed_ms, result}`.
	- `benchmark(f, iterations, args...)` → average ms per call.

- **Random helpers** (useful for custom fuzzing / testing)
	- `rng()` global `std::mt19937`.
	- `rand_int(lo, hi)`, `shuffle(vec)`.

## Runner (`main.cpp`)

- `Solutions` class holds input lines and exposes `part1()` and `part2()`.
- `main`:
	- Reads path from `argv[1]` or falls back to `"input.txt"`.
	- Instantiates `Solutions`, runs `part1` and `part2` once with `time_call`.
	- Prints answers and times, then runs a simple benchmark with configurable iterations.

Replace the placeholder logic inside `Solutions::part1` / `part2` with your per-day solutions. You can freely add member variables to `Solutions` as reusable state.

## Build with CMake

```bash
cmake -S . -B build
cmake --build build --config Release
```
