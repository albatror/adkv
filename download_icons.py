import requests
import os

icons = {
    "light_ammo.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/1/18/Light_Ammo.png/revision/latest",
    "energy_ammo.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/1/1a/Energy_Ammo.png/revision/latest",
    "shotgun_ammo.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/d/df/Shotgun_Ammo.png/revision/latest",
    "heavy_ammo.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/0/0a/Heavy_Ammo.png/revision/latest",
    "sniper_ammo.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/4/4b/Sniper_Ammo.png/revision/latest",
    "med_kit.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/c/c2/Med_Kit.png/revision/latest",
    "shield_battery.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/a/a2/Shield_Battery.png/revision/latest",
    "ultimate_accelerant.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/c/c1/Ultimate_Accelerant.png/revision/latest",
    "r-301.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/d/d3/R-301_Carbine_Inventory_Icon.png/revision/latest",
    "flatline.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/a/a2/VK-47_Flatline_Inventory_Icon.png/revision/latest",
    "r-99.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/0/0a/R-99_SMG_Inventory_Icon.png/revision/latest",
    "alternator.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/b/b3/Alternator_SMG_Inventory_Icon.png/revision/latest",
    "peacekeeper.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/3/30/Peacekeeper_Inventory_Icon.png/revision/latest",
    "mastiff.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/4/4b/Mastiff_Shotgun_Inventory_Icon.png/revision/latest",
    "wingman.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/2/25/Wingman_Inventory_Icon.png/revision/latest",
    "kraber.png": "https://static.wikia.nocookie.net/apexlegends_gamepedia_en/images/f/f6/Kraber_.50-Cal_Sniper_Inventory_Icon.png/revision/latest"
}

output_dir = "apex_guest/Client/Client/icons"
os.makedirs(output_dir, exist_ok=True)

headers = {
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36"
}

for name, url in icons.items():
    try:
        response = requests.get(url, headers=headers)
        if response.status_code == 200:
            with open(os.path.join(output_dir, name), "wb") as f:
                f.write(response.content)
            print(f"Downloaded {name}")
        else:
            print(f"Failed to download {name}: {response.status_code}")
    except Exception as e:
        print(f"Error downloading {name}: {e}")
