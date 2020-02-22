import json
import sys

def read_json(filename):
    with open(filename) as fin:
        return json.load(fin)

if __name__ == '__main__':
    min_coverage_value = float(sys.argv[1])
    report_path = sys.argv[2]
    report = read_json(report_path)

    stats_to_check = ['lines', 'functions', 'regions']
    coverage_info = {}

    for file_data in report['data'][0]['files']:
        if 'phonebook.h' not in file_data['filename']:
            continue
        for stat in stats_to_check:
            coverage_info[stat] = file_data['summary'][stat]['percent']

    status = 0
    for stat in sorted(coverage_info.keys()):
        cur_status = 'OK!'
        if coverage_info[stat] < min_coverage_value:
            cur_status = 'Fail'
            status = 1
        print('{} coverage is {}: {}'.format(stat, coverage_info[stat], cur_status))

    if status:
        print('Code coverage is too low')
    sys.exit(status)
