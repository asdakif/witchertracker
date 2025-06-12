import os
import sys
import checker
import multiprocessing
from pathlib import Path

TIME_LIMIT = 30  # seconds

def run_checker_in_process(executable, input_path, my_output_path, expected_output_path, return_queue):
    try:
        score = checker.run(executable, input_path, my_output_path, expected_output_path)
    except Exception as e:
        score = 0.0
    return_queue.put(score)

def get_grade(executable: str, test_cases_folder: str) -> float:
    if not os.path.exists('my-outputs'):
        os.makedirs('my-outputs')

    os.system('make')

    total_score = 0.0
    count = 0

    for input_file in sorted(os.listdir(test_cases_folder)):
        if not input_file.startswith('input'):
            continue

        output_file = input_file.replace('input', 'output')
        input_path = f'{test_cases_folder}/{input_file}'
        my_output_path = f'my-outputs/{output_file}'
        expected_output_path = f'{test_cases_folder}/{output_file}'

        return_queue = multiprocessing.Queue()
        p = multiprocessing.Process(target=run_checker_in_process, args=(
            executable, input_path, my_output_path, expected_output_path, return_queue
        ))
        p.start()
        p.join(TIME_LIMIT)

        if p.is_alive():
            p.terminate()
            p.join()
            print(f"\033[91m⏱ Timeout on '{input_file}' — assigning 0.\033[0m", file=sys.stderr)
            test_grade = 0.0
        else:
            try:
                test_grade = return_queue.get(timeout=1)
            except:
                test_grade = 0.0

        total_score += test_grade
        count += 1
        print(f"\033[93mtest-case '{input_file}': {test_grade * 100:.2f} points\033[0m", file=sys.stderr)

    return (total_score * 100) / count if count > 0 else 0.0


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Usage: python grader.py <executable> <test_cases_folder>')
        sys.exit(1)
    
    grade = get_grade(sys.argv[1], sys.argv[2])

    print(f'\033[92mGrade = {grade:.2f}\033[0m', file=sys.stderr)
    print(f'{grade:.2f}')
