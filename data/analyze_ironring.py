#!/usr/bin/python3
from glob import glob
import json
""" Analyze the ctrl-c data in iron_ringlevel3 and summarizes the affixes repartition, to IronRingData.json
"""

def is_prefix(mod):
    matches = ["maximum Life", "maximum Mana", "maximum Energy Shield", "Fire Damage to Attacks", "Cold Damage to Attacks", "Lightning Damage to Attacks", "Evasion Rating"]
    return any(x in mod for x in matches)

def find_mods(data):
    parts = data.split("--------")
    return parts[-1]

# Return (prefix, suffix) for a single file
def process_file(data):
    prefix = 0
    suffix = 0
    for mod in find_mods(data).split("\n"):
        if not mod: # Empty lines at start/finish
            continue
        if is_prefix(mod):
            prefix += 1
        else:
            suffix += 1
    return prefix, suffix

results = {}
pref = 0
suff = 0
total_items = 0
for filename in glob("ironring_level3/*.txt"):
    with open(filename, "r") as f:
        data = f.read()
        if len(data) < 10:
            print(f"{filename} is too short, skipping")
            continue
        affixes = process_file(data)
        pref += affixes[0]
        suff += affixes[1]
        total_items += 1
        try:
            results[affixes] += 1
        except:
            results[affixes] = 1

four_mods = results[(2, 2)] + results[(1, 3)] + results[(3, 1)]
five_mods = results[(2, 3)] + results[(3, 2)]
six_mods = results[(3, 3)]
out = {"total": total_items,
       "four_mods": four_mods, "five_mods": five_mods, "six_mods": six_mods,
       "prefixes": pref, "suffixes": suff,
       "raw_data": {"1, 3": results[(1, 3)], "2, 2": results[(2, 2)], "3, 1": results[(3, 1)], "2, 3": results[(2, 3)], "3, 2": results[(3, 2)], "3, 3": results[(3, 3)]}
}
with open("IronRingData.json", "w") as f:
    json.dump(out, f, indent=2)
