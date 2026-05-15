import subprocess
import json
from typing import List, Dict, Any

TIMEOUT_SECONDS = 30


def run_single_command(command: str) -> Dict[str, Any]:
    """
    Executes a single shell command and returns structured result.
    """

    try:
        result = subprocess.run(
            command,
            shell=True,
            capture_output=True,
            text=True,
            timeout=TIMEOUT_SECONDS
        )

        status = "success" if result.returncode == 0 else "Failed"

        output = result.stdout.strip()
        error = result.stderr.strip()

        # Handle silent failures
        if status == "Failed" and not error:
            error = (
                f"Command '{command}' failed "
                f"with return code {result.returncode}"
            )

        return {
            command: {
                "output": output,
                "error": error,
                "status": status
            }
        }

    except subprocess.TimeoutExpired:
        return {
            command: {
                "output": "",
                "error": (
                    f"Command '{command}' timed out "
                    f"after {TIMEOUT_SECONDS} seconds"
                ),
                "status": "Failed"
            }
        }

    except Exception as exception:
        return {
            command: {
                "output": "",
                "error": str(exception),
                "status": "Failed"
            }
        }


def execute_commands(commands: List[str]) -> List[Dict[str, Any]]:
    """
    Executes unique shell commands and returns their results.
    """

    # Remove duplicate commands while preserving order
    unique_commands = list(dict.fromkeys(commands))

    results = []

    for command in unique_commands:
        results.append(run_single_command(command))

    return results


if __name__ == "__main__":

    commands = [
        "dir",
        "cd",
        "whoami",
        "hostname",
        "invalid_command",
        "echo Hello World"
    ]

    print("Executing Commands...\n")

    execution_results = execute_commands(commands)

    print(json.dumps(execution_results, indent=4))