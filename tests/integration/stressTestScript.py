import subprocess
from pathlib import Path


#################################################################################################
# TO RUN THIS YOU MUST HAVE hey.exe AT THE SERVER ROOT DIRECTORY

# spceifiy the number of conccurent users below, and if you are using  a laptop its better to have it plugged for best results 
#################################################################################################


HEY_PATH = Path(__file__).resolve().parents[2] / "hey.exe"
TARGET_URL = "http://localhost:8080/"
NUM_REQUESTS = 1_000_000


def run_hey( conccurency: int ) -> str:
    cmd = [
        HEY_PATH,
        "-n", str(NUM_REQUESTS),
        "-c", str(conccurency),
        TARGET_URL
    ]
    result = subprocess.run(
        cmd,
        capture_output = True,
        text = True
    )
    if result.returncode != 0:
        print(f"[warning] hey exited with code {result.returncode}")
        print(result.stderr)
    return result.stdout




if __name__ == "__main__":
    output = run_hey(conccurency=100) # change it to however many users you like 
    print(output)