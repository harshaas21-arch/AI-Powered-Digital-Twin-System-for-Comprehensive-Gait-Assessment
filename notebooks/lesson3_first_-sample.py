from pathlib import Path

# -------------------------------
# Locate the dataset
# -------------------------------

project_root = Path(__file__).resolve().parent.parent
dataset_path = project_root / "dataset" / "UCI-HAR Dataset"

# -------------------------------
# Training files
# -------------------------------

x_train_file = dataset_path / "train" / "X_train.txt"
y_train_file = dataset_path / "train" / "y_train.txt"

# -------------------------------
# Read first training sample
# -------------------------------

with open(x_train_file, "r") as file:
    first_sample = file.readline()

with open(y_train_file, "r") as file:
    first_label = file.readline()

# -------------------------------
# Display results
# -------------------------------

features = first_sample.split()

print("Number of features in one sample:", len(features))

print("\nFirst 10 features:")

for value in features[:10]:
    print(value)

print("\nCorrect Label:")

print(first_label.strip())