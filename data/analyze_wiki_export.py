from glob import glob
import json
"""
Analyze the wiki export (generated with pypoe's exporter) to generate a list of crafting base.
Use the wiki_export/ folder, and the SimpleTags.json file to convert text tags into int tags
"""


def item_tag_to_int(all_tags, tag):
    for  x in all_tags:
        if x['Id'] == tag:
            return x['rowId']
    return -1

def tags_to_int(all_tags, tags):
    return [item_tag_to_int(all_tags, tag) for tag in tags]

# Find the relevant information (class_id, tags, name) in a wiki export (.txt) file

def find_information(data):
    class_id = ""
    tags = ""
    name = ""
    for line in data.split("\n"):
        if "|class_id" in line:
            class_id = line.split(" = ")[1].strip()
        if "|name" in line:
            name = line.split(" = ")[1].strip()
        if "|tags" in line:
            tags = line.split(" = ")[1].strip().split(", ")
    return name, class_id, tags

# This data has been manually entered (could probably be automatized)
good_class_id = ['Amulet', 'Sceptre', 'Shield', 'Two Hand Axe','Ring', 'Quiver', 'Staff', 'Claw', 'Two Hand Sword', 'Two Hand Mace', 'One Hand Mace', 'Gloves', 'Bow', 'Helmet', 'Dagger', 'Wand', 'Thrusting One Hand Sword', 'Body Armour', 'AbyssJewel', 'One Hand Axe', 'Boots', 'Belt','Jewel', 'One Hand Sword']
shaper_tag = {'Amulet': 256, 'Sceptre': 284, 'Shield': 264, 'Two Hand Axe': 278, 'Ring': 258, 'Quiver': 262, 'Staff': 282, 'Claw': 272, 'Two Hand Sword': 266, 'Two Hand Mace': 280, 'One Hand Mace': 270, 'Gloves': 250, 'Bow': 274, 'Helmet': 252, 'Dagger': 276, 'Wand': 268, 'Thrusting One Hand Sword': 248, 'Body Armour': 254, 'One Hand Axe': 268, 'Boots': 246, 'Belt': 260, 'One Hand Sword': 248}


out = []
with open("SimpleTags.json", "r") as f:
    all_tags = json.load(f)

for filename in glob('wiki_export/*.txt'):
    with open(filename, 'r') as f:
        data = f.read()
    name, class_id, tags = find_information(data)
    tags_int = tags_to_int(all_tags, tags)
    if class_id in good_class_id:
        obj = {"name": name, "class_id": class_id, "tags_str": tags, "tags_int": tags_int}
        if class_id in shaper_tag:
            obj["shaper_tag"] = shaper_tag[class_id]
            obj["elder_tag"] = shaper_tag[class_id] + 1
        out.append(obj)

with open("SimpleBaseItemTypes.json", "w") as f:
    json.dump(out, f, indent=2)



