import json
import os
import sys

def merge_settings():
    active_file_dir = sys.argv[1]
    vscode_settings_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'settings.json')
    custom_settings_path = os.path.join(active_file_dir, '.vscode', 'settings.json')

    try:
        with open(custom_settings_path, 'r') as f, open(vscode_settings_path, 'r') as f_vscode:
            vscode_settings = json.load(f_vscode)
            custom_settings = json.load(f)

        if 'C_ARGS_BASE' not in custom_settings['tasks']:
            print('C_ARGS_BASE is not defined in custom_settings. Skipping merging.')
        else:
            vscode_settings['tasks']['C_ARGS_BASE'] = ['\t'.join(custom_settings['tasks']['C_ARGS_BASE'])]

        if 'C_ARGS' not in custom_settings['tasks']:
            print('C_ARGS is not defined in custom_settings. Skipping merging.')
        else:
            vscode_settings['tasks']['C_ARGS'] = ['\t'.join(custom_settings['tasks']['C_ARGS'])]

        if 'CPP_ARGS_BASE' not in custom_settings['tasks']:
            print('CPP_ARGS_BASE is not defined in custom_settings. Skipping merging.')
        else:
            vscode_settings['tasks']['CPP_ARGS_BASE'] = ['\t'.join(custom_settings['tasks']['CPP_ARGS_BASE'])]

        if 'CPP_ARGS' not in custom_settings['tasks']:
            print('CPP_ARGS is not defined in custom_settings. Skipping merging.')
        else:
            vscode_settings['tasks']['CPP_ARGS'] = ['\t'.join(custom_settings['tasks']['CPP_ARGS'])]

        with open(vscode_settings_path, 'w') as f_vscode:
            json.dump(vscode_settings, f_vscode, indent=4)
            print('Settings merged successfully!')

    except FileNotFoundError:
        print('settings.json file does not exist in the custom_settings_path. Exiting.')

if __name__ == "__main__":
    merge_settings()
