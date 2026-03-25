import requests
import os
import re

items = {
    "G7_SCOUT": "G7_Scout",
    "SPITFIRE": "M600_Spitfire",
    "R-301": "R-301_Carbine",
    "R-99": "R-99_SMG",
    "ALTERNATOR": "Alternator_SMG",
    "RE-45": "RE-45_Auto",
    "P2020": "P2020",
    "VOLT": "Volt_SMG",
    "L-STAR": "L-STAR_EMG",
    "DEVOTION": "Devotion_LMG",
    "HAVOC": "HAVOC_Rifle",
    "TRIPLE_TAKE": "Triple_Take",
    "MOZAMBIQUE": "Mozambique_Shotgun",
    "EVA-8": "EVA-8_Auto",
    "MASTIFF": "Mastiff_Shotgun",
    "PEACEKEEPER": "Peacekeeper",
    "FLATLINE": "VK-47_Flatline",
    "HEMLOK": "Hemlok_Burst_AR",
    "PROWLER": "Prowler_Burst_PDW",
    "30-30": "30-30_Repeater",
    "RAMPAGE": "Rampage_LMG",
    "C.A.R.": "C.A.R._SMG",
    "BOCEK": "Bocek_Compound_Bow",
    "SENTINEL": "Sentinel",
    "CHARGE_RIFLE": "Charge_Rifle",
    "LONGBOW": "Longbow_DMR",
    "WINGMAN": "Wingman",
    "KRABER": "Kraber_.50-Cal_Sniper",
    "LIGHT_AMMO": "Light_Ammo",
    "ENERGY_AMMO": "Energy_Ammo",
    "SHOTGUN_AMMO": "Shotgun_Ammo",
    "HEAVY_AMMO": "Heavy_Ammo",
    "SNIPER_AMMO": "Sniper_Ammo",
    "MED_KIT": "Med_Kit",
    "SHIELD_BATTERY": "Shield_Battery",
    "ULTIMATE_ACCEL": "Ultimate_Accelerant",
    "HEAT_SHIELD": "Heat_Shield",
    "EVAC_TOWER": "Evac_Tower"
}

base_url = "https://apexlegends.wiki.gg/wiki/"
output_dir = "apex_guest/Client/Client/icons"
os.makedirs(output_dir, exist_ok=True)

for item, wiki_name in items.items():
    try:
        response = requests.get(base_url + wiki_name)
        if response.status_code == 200:
            # Look for file link
            match = re.search(r'href="/wiki/File:([^"]+Inventory_Icon\.png)"', response.text)
            if not match:
                match = re.search(r'href="/wiki/File:([^"]+Icon\.png)"', response.text)
            if not match:
                match = re.search(r'href="/wiki/File:([^"]+\.png)"', response.text)

            if match:
                file_page = match.group(1)
                res_file = requests.get(base_url + "File:" + file_page)
                img_match = re.search(r'src="(https://apexlegends\.wiki\.gg/images/[^"]+\.png)"', res_file.text)
                if img_match:
                    icon_url = img_match.group(1)
                    img_data = requests.get(icon_url).content
                    with open(os.path.join(output_dir, f"{item.lower()}.png"), "wb") as f:
                        f.write(img_data)
                    print(f"Downloaded {item}")
                else:
                    print(f"Could not find direct image for {item}")
            else:
                print(f"Could not find file link for {item}")
        else:
            print(f"Failed to find page for {item} ({wiki_name})")
    except Exception as e:
        print(f"Error downloading {item}: {e}")
