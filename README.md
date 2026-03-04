# STAR DST Framework

This repository provides a **minimal analysis framework for the STAR experiment**.  
It is intended as a **tutorial for new STAR collaborators** who want to learn how to read MuDst/PicoDst files, run a simple analysis maker, and optionally submit jobs to the STAR batch system.

The framework is intentionally simple so that users can easily modify it for their own analyses.

---

# Quick Start

Clone the repository and load the STAR environment:

```bash
starver your-version
git clone https://github.com/DanielTorres98/STAR-dst-framework
cd STAR-dst-framework
```

Make the helper script executable:

```bash
chmod +x get_file_list.sh
```

Generate a list of input files:

```bash
./get_file_list.sh
```

Run the analysis inside your root container:

```bash
root4star -b femtoDst_maker.C
```

---

# Repository Structure

```
StRoot/                  Analysis maker source code
femtoDst_maker.C         Main macro used to run the analysis
load.C                   Loads required STAR libraries
get_file_list.sh         Script to generate list of input files
submit_jobs.xml          Example XML configuration for batch jobs
```

---

# Requirements

Before running the analysis, make sure the STAR environment is loaded:

```bash
starver your-STAR-version
```

You must also have access to **MuDst or PicoDst files**.

---

# Getting Input Files

The script

```
get_file_list.sh
```

creates a list of input files used by the analysis.

When testing, ensure that **the triggers you want to analyze are present in the files you download**.

---

# Modifying the Analysis

The analysis code lives inside

```
StRoot/StMu2AnaMaker
```

Users should modify this code according to their analysis needs.  
Typical changes include:

- event selection  
- trigger selection  
- track quality cuts  
- histogram definitions  
- output tree variables  

---

# Running the Analysis Locally

Once your modifications are complete, run:

```bash
root4star -b femtoDst_maker.C
```

This will:

1. Load the STAR libraries  
2. Process the input MuDst/PicoDst files  
3. Produce an output ROOT file containing the analysis results  

---

# Submitting Jobs to the STAR Batch System

The analysis can also be submitted to the STAR batch system.

Before submitting, edit the paths inside:

```
submit_jobs.xml
```

Update the relevant fields such as:

- input file paths  
- output directory  
- STAR library version  

Then submit the jobs with:

```bash
star-submit-beta submit_jobs.xml
```

---

# Notes

- Always verify that your **input dataset contains the triggers you want to analyze**.
- This repository is intended as a **minimal tutorial framework**, not a full production analysis chain.
- Users are encouraged to adapt the analysis maker for their specific physics studies.

---

# Author

Daniel Torres Valladares  
Rice University — STAR Collaboration
