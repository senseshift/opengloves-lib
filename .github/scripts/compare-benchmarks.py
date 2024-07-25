import xml.etree.ElementTree as ET
import sys


def parse_benchmark_results(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    results = {}

    for testcase in root.findall('TestCase'):
        test_name = testcase.get('name')
        for benchmark in testcase.findall('BenchmarkResults'):
            benchmark_name = benchmark.get('name')
            mean = benchmark.find('mean').get('value')
            stddev = benchmark.find('standardDeviation').get('value')

            full_test_name = f"{test_name}/{benchmark_name}"
            results[full_test_name] = (float(mean), float(stddev))

    return results


def calculate_percentage_change(base, pr):
    if base is None:
        return "N/A", "N/A", pr[0], pr[1], "N/A"
    if pr is None:
        return base[0], base[1], "N/A", "N/A", "N/A"

    base_mean, base_stddev = base
    pr_mean, pr_stddev = pr

    change = ((pr_mean - base_mean) / base_mean) * 100 if base_mean != 0 else float('inf')

    return base_mean, base_stddev, pr_mean, pr_stddev, change


def generate_markdown_table(base_results, pr_results):
    all_tests = set(base_results.keys()).union(set(pr_results.keys()))

    table_header = "| Test | Base | PR | % |\n|------|------|----|---|\n"
    table_rows = []

    for test_name in all_tests:
        base = base_results.get(test_name)
        pr = pr_results.get(test_name)

        base_mean, base_stddev, pr_mean, pr_stddev, change = calculate_percentage_change(base, pr)
        base_str = f"{base_mean:.2f}±{base_stddev:.2f}ns" if base_mean != "N/A" else "N/A"
        pr_str = f"{pr_mean:.2f}±{pr_stddev:.2f}ns" if pr_mean != "N/A" else "N/A"
        change_str = f"{change:+.2f}%" if change != "N/A" else "N/A"

        row = f"| {test_name} | {base_str} | {pr_str} | {change_str} |\n"
        table_rows.append(row)

    return table_header + ''.join(table_rows)


def main(base_file, pr_file):
    base_results = parse_benchmark_results(base_file)
    pr_results = parse_benchmark_results(pr_file)

    markdown_table = generate_markdown_table(base_results, pr_results)

    print("## Benchmark results comparison:\n")
    print("<details>")
    print("  <summary>Click to expand</summary>\n")
    print(markdown_table)
    print("</details>")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <base_xml_path> <pr_xml_path>")
    else:
        base_file = sys.argv[1]
        pr_file = sys.argv[2]
        main(base_file, pr_file)
