#pragma once
#include <imgui.h>

struct ApexColorAtlasPage {
    const char* file_path;
    int width;
    int height;
};

struct ApexColorAtlasIcon {
    const char* name;
    const char* file_name;
    int page;
    ImVec2 size;
    ImVec2 uv0;
    ImVec2 uv1;
};

static constexpr int kApexColorAtlasPageCount = 3;
static constexpr int kApexColorAtlasIconCount = 571;

static const ApexColorAtlasPage kApexColorAtlasPages[] = {
    { "pages/apex_icons_page_0.png", 2048, 2048 },
    { "pages/apex_icons_page_1.png", 2048, 2048 },
    { "pages/apex_icons_page_2.png", 2048, 2048 },
};

static const ApexColorAtlasIcon kApexColorAtlasIcons[] = {
    { "1x-digital-threat", "1x Digital Threat.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.004883f), ImVec2(0.057617f, 0.057617f) },
    { "1x-hcog-classic", "1x HCOG Classic.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.004883f), ImVec2(0.120117f, 0.057617f) },
    { "1x-holo", "1x Holo.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.004883f), ImVec2(0.182617f, 0.057617f) },
    { "1x-2x-variable-holo", "1x-2x Variable Holo.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.004883f), ImVec2(0.245117f, 0.057617f) },
    { "2x-hcog-bruiser", "2x HCOG Bruiser.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.004883f), ImVec2(0.307617f, 0.057617f) },
    { "2x-4x-variable-aog", "2x-4x Variable AOG.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.004883f), ImVec2(0.370117f, 0.057617f) },
    { "30-30-repeater-icon", "30-30 Repeater Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.004883f), ImVec2(0.432617f, 0.057617f) },
    { "3x-hcog-ranger", "3x HCOG _Ranger_.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.004883f), ImVec2(0.495117f, 0.057617f) },
    { "4x-10x-digital-sniper-threat", "4x-10x Digital Sniper Threat.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.004883f), ImVec2(0.557617f, 0.057617f) },
    { "4x-8x-variable-sniper", "4x-8x Variable Sniper.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.004883f), ImVec2(0.620117f, 0.057617f) },
    { "6x-sniper", "6x Sniper.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.004883f), ImVec2(0.682617f, 0.057617f) },
    { "a-13-sentry-icon", "A-13 Sentry Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.004883f), ImVec2(0.745117f, 0.057617f) },
    { "activity", "Activity.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.004883f), ImVec2(0.807617f, 0.057617f) },
    { "aftermarket-icon", "Aftermarket Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.004883f), ImVec2(0.870117f, 0.057617f) },
    { "airdrop-escalation-icon", "Airdrop Escalation Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.004883f), ImVec2(0.932617f, 0.057617f) },
    { "algs-icon", "ALGS Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.004883f), ImVec2(0.995117f, 0.057617f) },
    { "alternator-smg-icon", "Alternator SMG Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.067383f), ImVec2(0.057617f, 0.120117f) },
    { "amped-cover", "Amped Cover.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.067383f), ImVec2(0.120117f, 0.120117f) },
    { "anniversary-event-icon", "Anniversary Event Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.067383f), ImVec2(0.182617f, 0.120117f) },
    { "anniversary-icon", "Anniversary Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.067383f), ImVec2(0.245117f, 0.120117f) },
    { "anvil-receiver", "Anvil Receiver.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.067383f), ImVec2(0.307617f, 0.120117f) },
    { "apex-coins", "Apex Coins.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.067383f), ImVec2(0.370117f, 0.120117f) },
    { "apex-pack-icon", "Apex Pack Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.067383f), ImVec2(0.432617f, 0.120117f) },
    { "apex-predators", "Apex Predators.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.067383f), ImVec2(0.495117f, 0.120117f) },
    { "arc-snare", "Arc Snare.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.067383f), ImVec2(0.557617f, 0.120117f) },
    { "arc-star", "Arc Star.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.067383f), ImVec2(0.620117f, 0.120117f) },
    { "arenas-icon", "Arenas Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.067383f), ImVec2(0.682617f, 0.120117f) },
    { "armed-and-dangerous-icon", "Armed and Dangerous Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.067383f), ImVec2(0.745117f, 0.120117f) },
    { "armor-regen-icon", "Armor Regen Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.067383f), ImVec2(0.807617f, 0.120117f) },
    { "arrows", "Arrows.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.067383f), ImVec2(0.870117f, 0.120117f) },
    { "ash-icon", "Ash Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.067383f), ImVec2(0.932617f, 0.120117f) },
    { "assassin-s-instinct", "Assassin's Instinct.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.067383f), ImVec2(0.995117f, 0.120117f) },
    { "assault-class", "Assault Class.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.129883f), ImVec2(0.057617f, 0.182617f) },
    { "assists-icon", "Assists Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.129883f), ImVec2(0.120117f, 0.182617f) },
    { "auto-banners-icon", "Auto Banners Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.129883f), ImVec2(0.182617f, 0.182617f) },
    { "auto-icon", "Auto Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.129883f), ImVec2(0.245117f, 0.182617f) },
    { "awakening-icon", "Awakening Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.129883f), ImVec2(0.307617f, 0.182617f) },
    { "backpack-lvl1", "Backpack lvl1.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.129883f), ImVec2(0.370117f, 0.182617f) },
    { "backpack-lvl2", "Backpack lvl2.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.129883f), ImVec2(0.432617f, 0.182617f) },
    { "backpack-lvl3", "Backpack lvl3.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.129883f), ImVec2(0.495117f, 0.182617f) },
    { "badge-fight-or-fright", "Badge Fight or Fright.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.129883f), ImVec2(0.557617f, 0.182617f) },
    { "badges-icon", "Badges Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.129883f), ImVec2(0.620117f, 0.182617f) },
    { "ballistic-icon", "Ballistic Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.129883f), ImVec2(0.682617f, 0.182617f) },
    { "bangalore-icon", "Bangalore Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.129883f), ImVec2(0.745117f, 0.182617f) },
    { "bangalore-mobile-stat-tracker-icon", "Bangalore Mobile Stat Tracker Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.129883f), ImVec2(0.807617f, 0.182617f) },
    { "bangalore-thematic-pack-icon", "Bangalore Thematic Pack Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.129883f), ImVec2(0.870117f, 0.182617f) },
    { "banner-frame-icon", "Banner Frame Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.129883f), ImVec2(0.932617f, 0.182617f) },
    { "banner-pose-icon", "Banner Pose Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.129883f), ImVec2(0.995117f, 0.182617f) },
    { "barrel-stabilizer-lvl4", "Barrel Stabilizer lvl4.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.192383f), ImVec2(0.057617f, 0.245117f) },
    { "barrel-stabilizer", "Barrel Stabilizer.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.192383f), ImVec2(0.120117f, 0.245117f) },
    { "barricade", "Barricade.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.192383f), ImVec2(0.182617f, 0.245117f) },
    { "battle-pass-level", "Battle Pass Level.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.192383f), ImVec2(0.245117f, 0.245117f) },
    { "battle-royale", "Battle Royale.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.192383f), ImVec2(0.307617f, 0.245117f) },
    { "beast-of-prey-icon", "Beast of Prey Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.192383f), ImVec2(0.370117f, 0.245117f) },
    { "beast-of-prey", "Beast of Prey.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.192383f), ImVec2(0.432617f, 0.245117f) },
    { "beast-of-the-hunt", "Beast of the Hunt.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.192383f), ImVec2(0.495117f, 0.245117f) },
    { "black-hole", "Black Hole.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.192383f), ImVec2(0.557617f, 0.245117f) },
    { "black-market-boutique", "Black Market Boutique.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.192383f), ImVec2(0.620117f, 0.245117f) },
    { "bloodhound-icon", "Bloodhound Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.192383f), ImVec2(0.682617f, 0.245117f) },
    { "bloodhound-mobile-stat-tracker-icon", "Bloodhound Mobile Stat Tracker Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.192383f), ImVec2(0.745117f, 0.245117f) },
    { "bloodhound-thematic-pack-icon", "Bloodhound Thematic Pack Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.192383f), ImVec2(0.807617f, 0.245117f) },
    { "bocek-compound-bow-icon", "Bocek Compound Bow Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.192383f), ImVec2(0.870117f, 0.245117f) },
    { "body-shield", "Body Shield.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.192383f), ImVec2(0.932617f, 0.245117f) },
    { "boosted-loader", "Boosted Loader.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.192383f), ImVec2(0.995117f, 0.245117f) },
    { "burglar-s-best-friend", "Burglar's Best Friend.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.254883f), ImVec2(0.057617f, 0.307617f) },
    { "burrell-defense", "Burrell Defense.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.254883f), ImVec2(0.120117f, 0.307617f) },
    { "burst-icon", "Burst Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.254883f), ImVec2(0.182617f, 0.307617f) },
    { "c-a-r-smg-icon", "C.A.R. SMG Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.254883f), ImVec2(0.245117f, 0.307617f) },
    { "camo-credits-icon", "Camo Credits Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.254883f), ImVec2(0.307617f, 0.307617f) },
    { "care-package", "Care Package.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.254883f), ImVec2(0.370117f, 0.307617f) },
    { "castle-wall", "Castle Wall.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.254883f), ImVec2(0.432617f, 0.307617f) },
    { "catalyst-icon", "Catalyst Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.254883f), ImVec2(0.495117f, 0.307617f) },
    { "caustic-icon", "Caustic Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.254883f), ImVec2(0.557617f, 0.307617f) },
    { "caustic-mobile-stat-tracker-icon", "Caustic Mobile Stat Tracker Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.254883f), ImVec2(0.620117f, 0.307617f) },
    { "celestial-sunrise", "Celestial Sunrise.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.254883f), ImVec2(0.682617f, 0.307617f) },
    { "challenge-stars", "Challenge Stars.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.254883f), ImVec2(0.745117f, 0.307617f) },
    { "champion-edition-icon", "Champion Edition Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.254883f), ImVec2(0.807617f, 0.307617f) },
    { "chaos-theory-event-icon", "Chaos Theory Event Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.254883f), ImVec2(0.870117f, 0.307617f) },
    { "chaos-theory-icon", "Chaos Theory Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.254883f), ImVec2(0.932617f, 0.307617f) },
    { "charge-rifle-icon", "Charge Rifle Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.254883f), ImVec2(0.995117f, 0.307617f) },
    { "charms-icon", "Charms Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.317383f), ImVec2(0.057617f, 0.370117f) },
    { "charms-thematic-pack-icon", "Charms Thematic Pack Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.317383f), ImVec2(0.120117f, 0.370117f) },
    { "chevrex", "Chevrex.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.317383f), ImVec2(0.182617f, 0.370117f) },
    { "choke-active-icon", "Choke Active Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.317383f), ImVec2(0.245117f, 0.370117f) },
    { "cleo-recovery-council", "Cleo Recovery Council.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.317383f), ImVec2(0.307617f, 0.370117f) },
    { "club-coins", "Club Coins.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.317383f), ImVec2(0.370117f, 0.370117f) },
    { "club-icon", "Club Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.317383f), ImVec2(0.432617f, 0.370117f) },
    { "combat-revive", "Combat Revive.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.317383f), ImVec2(0.495117f, 0.370117f) },
    { "community-content-icon", "Community Content Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.317383f), ImVec2(0.557617f, 0.370117f) },
    { "conduit-icon", "Conduit Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.317383f), ImVec2(0.620117f, 0.370117f) },
    { "control-icon", "Control Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.317383f), ImVec2(0.682617f, 0.370117f) },
    { "control", "Control.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.317383f), ImVec2(0.745117f, 0.370117f) },
    { "controller-class", "Controller Class.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.317383f), ImVec2(0.807617f, 0.370117f) },
    { "cracked-talon", "Cracked Talon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.317383f), ImVec2(0.870117f, 0.370117f) },
    { "crafting-evo", "Crafting Evo.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.317383f), ImVec2(0.932617f, 0.370117f) },
    { "crafting-icon", "Crafting Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.317383f), ImVec2(0.995117f, 0.370117f) },
    { "crafting-metals", "Crafting Metals.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.379883f), ImVec2(0.057617f, 0.432617f) },
    { "crypto-icon", "Crypto Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.379883f), ImVec2(0.120117f, 0.432617f) },
    { "crypto-symbol-icon", "Crypto Symbol Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.379883f), ImVec2(0.182617f, 0.432617f) },
    { "crypto-thematic-pack-icon", "Crypto Thematic Pack Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.379883f), ImVec2(0.245117f, 0.432617f) },
    { "d-o-c-heal-drone", "D.O.C. Heal Drone.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.379883f), ImVec2(0.307617f, 0.432617f) },
    { "damage", "Damage.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.379883f), ImVec2(0.370117f, 0.432617f) },
    { "dark-depths-icon", "Dark Depths Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.379883f), ImVec2(0.432617f, 0.432617f) },
    { "dark-veil", "Dark Veil.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.379883f), ImVec2(0.495117f, 0.432617f) },
    { "deadeye-s-tempo-active-icon", "Deadeye's Tempo Active Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.379883f), ImVec2(0.557617f, 0.432617f) },
    { "deadeye-s-tempo", "Deadeye's Tempo.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.379883f), ImVec2(0.620117f, 0.432617f) },
    { "death-dynasty-icon", "Death Dynasty Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.379883f), ImVec2(0.682617f, 0.432617f) },
    { "death-dynasty", "Death Dynasty.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.379883f), ImVec2(0.745117f, 0.432617f) },
    { "death-totem", "Death Totem.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.379883f), ImVec2(0.807617f, 0.432617f) },
    { "defensive-bombardment", "Defensive Bombardment.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.379883f), ImVec2(0.870117f, 0.432617f) },
    { "defensive", "Defensive.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.379883f), ImVec2(0.932617f, 0.432617f) },
    { "deja-loot-icon", "Deja Loot Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.379883f), ImVec2(0.995117f, 0.432617f) },
    { "devotion-lmg-icon", "Devotion LMG Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.442383f), ImVec2(0.057617f, 0.495117f) },
    { "dimensional-rift", "Dimensional Rift.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.442383f), ImVec2(0.120117f, 0.495117f) },
    { "disruptor-rounds", "Disruptor Rounds.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.442383f), ImVec2(0.182617f, 0.495117f) },
    { "dome-of-protection", "Dome of Protection.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.442383f), ImVec2(0.245117f, 0.495117f) },
    { "doppelgangers", "Doppelgangers.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.442383f), ImVec2(0.307617f, 0.495117f) },
    { "double-tap-trigger", "Double Tap Trigger.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.442383f), ImVec2(0.370117f, 0.495117f) },
    { "double-time", "Double Time.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.442383f), ImVec2(0.432617f, 0.495117f) },
    { "dragon-company", "Dragon Company.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.442383f), ImVec2(0.495117f, 0.495117f) },
    { "dressed-to-kill-icon", "Dressed to Kill Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.442383f), ImVec2(0.557617f, 0.495117f) },
    { "dressed-to-kill", "Dressed to Kill.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.442383f), ImVec2(0.620117f, 0.495117f) },
    { "drone-emp", "Drone EMP.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.442383f), ImVec2(0.682617f, 0.495117f) },
    { "dual-shell", "Dual Shell.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.442383f), ImVec2(0.745117f, 0.495117f) },
    { "ea-icon", "EA Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.442383f), ImVec2(0.807617f, 0.495117f) },
    { "echo-icon", "ECHO Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.442383f), ImVec2(0.870117f, 0.495117f) },
    { "echo-relocation", "Echo Relocation.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.442383f), ImVec2(0.932617f, 0.495117f) },
    { "emote-icon", "Emote Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.442383f), ImVec2(0.995117f, 0.495117f) },
    { "emote-thematic-pack-icon", "Emote Thematic Pack Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.504883f), ImVec2(0.057617f, 0.557617f) },
    { "emslie-tactical-icon", "Emslie Tactical Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.504883f), ImVec2(0.120117f, 0.557617f) },
    { "energy-ammo", "Energy Ammo.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.504883f), ImVec2(0.182617f, 0.557617f) },
    { "energy-barricade", "Energy Barricade.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.504883f), ImVec2(0.245117f, 0.557617f) },
    { "eva-8-auto-icon", "EVA-8 Auto Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.504883f), ImVec2(0.307617f, 0.557617f) },
    { "evac-tower", "Evac Tower.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.504883f), ImVec2(0.370117f, 0.557617f) },
    { "event-apex-pack-icon", "Event Apex Pack Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.504883f), ImVec2(0.432617f, 0.557617f) },
    { "evo-shield-level-2", "Evo Shield Level 2.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.504883f), ImVec2(0.495117f, 0.557617f) },
    { "evo-shield-level-3", "Evo Shield Level 3.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.504883f), ImVec2(0.557617f, 0.557617f) },
    { "evo-shield-level-5", "Evo Shield Level 5.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.504883f), ImVec2(0.620117f, 0.557617f) },
    { "evo-shield", "Evo Shield.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.504883f), ImVec2(0.682617f, 0.557617f) },
    { "evolution-icon", "Evolution Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.504883f), ImVec2(0.745117f, 0.557617f) },
    { "exhibit", "Exhibit.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.504883f), ImVec2(0.807617f, 0.557617f) },
    { "extended-car-mag", "Extended CAR Mag.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.504883f), ImVec2(0.870117f, 0.557617f) },
    { "extended-energy-mag", "Extended Energy Mag.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.504883f), ImVec2(0.932617f, 0.557617f) },
    { "extended-heavy-mag", "Extended Heavy Mag.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.504883f), ImVec2(0.995117f, 0.557617f) },
    { "extended-light-mag", "Extended Light Mag.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.567383f), ImVec2(0.057617f, 0.620117f) },
    { "extended-sniper-mag", "Extended Sniper Mag.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.567383f), ImVec2(0.120117f, 0.620117f) },
    { "eye-for-quality", "Eye for Quality.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.567383f), ImVec2(0.182617f, 0.620117f) },
    { "eye-of-the-allfather", "Eye of the Allfather.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.567383f), ImVec2(0.245117f, 0.620117f) },
    { "fade-chip", "Fade Chip.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.567383f), ImVec2(0.307617f, 0.620117f) },
    { "fade-icon", "Fade Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.567383f), ImVec2(0.370117f, 0.620117f) },
    { "fade-mobile-stat-tracker-icon", "Fade Mobile Stat Tracker Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.567383f), ImVec2(0.432617f, 0.620117f) },
    { "fight-night-event-icon", "Fight Night Event Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.567383f), ImVec2(0.495117f, 0.620117f) },
    { "fight-night-icon", "Fight Night Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.567383f), ImVec2(0.557617f, 0.620117f) },
    { "fight-or-fright-2020-icon", "Fight or Fright 2020 Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.567383f), ImVec2(0.620117f, 0.620117f) },
    { "fight-or-fright-icon", "Fight or Fright Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.567383f), ImVec2(0.682617f, 0.620117f) },
    { "final-fantasy-vii", "Final Fantasy VII.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.567383f), ImVec2(0.745117f, 0.620117f) },
    { "flashback", "Flashback.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.567383f), ImVec2(0.807617f, 0.620117f) },
    { "flux", "Flux.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.567383f), ImVec2(0.870117f, 0.620117f) },
    { "focus-of-attention", "Focus of Attention.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.567383f), ImVec2(0.932617f, 0.620117f) },
    { "forged-shadows", "Forged Shadows.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.567383f), ImVec2(0.995117f, 0.620117f) },
    { "fortified", "Fortified.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.629883f), ImVec2(0.057617f, 0.682617f) },
    { "founders-pack-icon", "Founders Pack Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.629883f), ImVec2(0.120117f, 0.682617f) },
    { "frag-grenade", "Frag Grenade.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.629883f), ImVec2(0.182617f, 0.682617f) },
    { "frontier-corps-icon", "Frontier Corps Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.629883f), ImVec2(0.245117f, 0.682617f) },
    { "fuse-icon", "Fuse Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.629883f), ImVec2(0.307617f, 0.682617f) },
    { "g7-scout-icon", "G7 Scout Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.629883f), ImVec2(0.370117f, 0.682617f) },
    { "gaiden", "Gaiden.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.629883f), ImVec2(0.432617f, 0.682617f) },
    { "genesis-event-icon", "Genesis Event Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.629883f), ImVec2(0.495117f, 0.682617f) },
    { "genesis-icon", "Genesis Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.629883f), ImVec2(0.557617f, 0.682617f) },
    { "gibraltar-icon", "Gibraltar Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.629883f), ImVec2(0.620117f, 0.682617f) },
    { "gibraltar-mobile-icon", "Gibraltar Mobile Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.629883f), ImVec2(0.682617f, 0.682617f) },
    { "gibraltar-mobile-stat-tracker-icon", "Gibraltar Mobile Stat Tracker Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.629883f), ImVec2(0.745117f, 0.682617f) },
    { "gifted-ear", "Gifted Ear.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.629883f), ImVec2(0.807617f, 0.682617f) },
    { "golden-ticket", "Golden Ticket.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.629883f), ImVec2(0.870117f, 0.682617f) },
    { "grand-soiree-icon", "Grand Soiree Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.629883f), ImVec2(0.932617f, 0.682617f) },
    { "grappling-hook", "Grappling Hook.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.629883f), ImVec2(0.995117f, 0.682617f) },
    { "gravity-lift", "Gravity Lift.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.692383f), ImVec2(0.057617f, 0.745117f) },
    { "grenadier", "Grenadier.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.692383f), ImVec2(0.120117f, 0.745117f) },
    { "gun-run-icon", "Gun Run Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.692383f), ImVec2(0.182617f, 0.745117f) },
    { "gun-run", "Gun Run.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.692383f), ImVec2(0.245117f, 0.745117f) },
    { "gun-shield", "Gun Shield.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.692383f), ImVec2(0.307617f, 0.745117f) },
    { "hammerpoint-rounds", "Hammerpoint Rounds.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.692383f), ImVec2(0.370117f, 0.745117f) },
    { "hammond-robotics-icon", "Hammond Robotics Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.692383f), ImVec2(0.432617f, 0.745117f) },
    { "harbingers", "Harbingers.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.692383f), ImVec2(0.495117f, 0.745117f) },
    { "havoc-rifle-icon", "HAVOC Rifle Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.692383f), ImVec2(0.557617f, 0.745117f) },
    { "heart-seeker", "Heart Seeker.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.692383f), ImVec2(0.620117f, 0.745117f) },
    { "heat-shield", "Heat Shield.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.692383f), ImVec2(0.682617f, 0.745117f) },
    { "heavy-rounds-revved-up", "Heavy Rounds Revved Up.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.692383f), ImVec2(0.745117f, 0.745117f) },
    { "heavy-rounds", "Heavy Rounds.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.692383f), ImVec2(0.807617f, 0.745117f) },
    { "heavylight-rounds", "HeavyLight Rounds.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.692383f), ImVec2(0.870117f, 0.745117f) },
    { "heirloom-shards", "Heirloom Shards.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.692383f), ImVec2(0.932617f, 0.745117f) },
    { "helmet", "Helmet.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.692383f), ImVec2(0.995117f, 0.745117f) },
    { "hemlok-burst-ar-icon", "Hemlok Burst AR Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.754883f), ImVec2(0.057617f, 0.807617f) },
    { "holo-day-bash-icon", "Holo-Day Bash Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.754883f), ImVec2(0.120117f, 0.807617f) },
    { "holospray-icon", "Holospray Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.754883f), ImVec2(0.182617f, 0.807617f) },
    { "hop-up-slot", "Hop Up Slot.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.754883f), ImVec2(0.245117f, 0.807617f) },
    { "horizon-icon", "Horizon Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.754883f), ImVec2(0.307617f, 0.807617f) },
    { "hype-anthem", "Hype Anthem.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.754883f), ImVec2(0.370117f, 0.807617f) },
    { "icon-champion", "Icon Champion.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.754883f), ImVec2(0.432617f, 0.807617f) },
    { "icon-congestion", "Icon Congestion.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.754883f), ImVec2(0.495117f, 0.807617f) },
    { "icon-lag-compensation-error", "Icon Lag Compensation Error.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.754883f), ImVec2(0.557617f, 0.807617f) },
    { "icon-latency", "Icon Latency.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.754883f), ImVec2(0.620117f, 0.807617f) },
    { "icon-packet-loss", "Icon Packet Loss.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.754883f), ImVec2(0.682617f, 0.807617f) },
    { "icon-prediction-error", "Icon Prediction Error.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.754883f), ImVec2(0.745117f, 0.807617f) },
    { "icon-ucmd-delay", "Icon UCMD Delay.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.754883f), ImVec2(0.807617f, 0.807617f) },
    { "imc-icon-1", "IMC Icon 1.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.754883f), ImVec2(0.870117f, 0.807617f) },
    { "imc-storm-point-icon", "IMC Storm Point Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.754883f), ImVec2(0.932617f, 0.807617f) },
    { "imperial-guard-icon", "Imperial Guard Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.754883f), ImVec2(0.995117f, 0.807617f) },
    { "insider-knowledge", "Insider Knowledge.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.817383f), ImVec2(0.057617f, 0.870117f) },
    { "interception-pylon", "Interception Pylon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.817383f), ImVec2(0.120117f, 0.870117f) },
    { "interstellar-manufacturing-corporation", "Interstellar Manufacturing Corporation.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.817383f), ImVec2(0.182617f, 0.870117f) },
    { "into-the-void", "Into the Void.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.817383f), ImVec2(0.245117f, 0.870117f) },
    { "intro-quip-icon", "Intro Quip Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.817383f), ImVec2(0.307617f, 0.870117f) },
    { "iron-crown-icon", "Iron Crown Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.817383f), ImVec2(0.370117f, 0.870117f) },
    { "kill-leader-icon", "Kill Leader Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.817383f), ImVec2(0.432617f, 0.870117f) },
    { "kill-quip-icon", "Kill Quip Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.817383f), ImVec2(0.495117f, 0.870117f) },
    { "kills-icon", "Kills Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.817383f), ImVec2(0.557617f, 0.870117f) },
    { "kinetic-feeder", "Kinetic Feeder.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.817383f), ImVec2(0.620117f, 0.870117f) },
    { "knockdown-shield", "Knockdown Shield.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.817383f), ImVec2(0.682617f, 0.870117f) },
    { "knuckle-cluster", "Knuckle Cluster.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.817383f), ImVec2(0.745117f, 0.870117f) },
    { "kodai", "Kodai.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.817383f), ImVec2(0.807617f, 0.870117f) },
    { "kraber-50-cal-sniper-icon", "Kraber .50-Cal Sniper Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.817383f), ImVec2(0.870117f, 0.870117f) },
    { "l-star-emg-icon", "L-STAR EMG Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.817383f), ImVec2(0.932617f, 0.870117f) },
    { "laser-sight", "Laser Sight.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.817383f), ImVec2(0.995117f, 0.870117f) },
    { "lastimosa-armory", "Lastimosa Armory.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.879883f), ImVec2(0.057617f, 0.932617f) },
    { "launch-pad", "Launch Pad.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.879883f), ImVec2(0.120117f, 0.932617f) },
    { "legend-fragments", "Legend Fragments.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.879883f), ImVec2(0.182617f, 0.932617f) },
    { "legend-mastery-icon", "Legend Mastery Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.879883f), ImVec2(0.245117f, 0.932617f) },
    { "legend-tokens", "Legend Tokens.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.879883f), ImVec2(0.307617f, 0.932617f) },
    { "legendary-hunt-icon", "Legendary Hunt Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.879883f), ImVec2(0.370117f, 0.932617f) },
    { "life-of-the-party", "Life of the Party.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.879883f), ImVec2(0.432617f, 0.932617f) },
    { "lifeline-icon", "Lifeline Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.879883f), ImVec2(0.495117f, 0.932617f) },
    { "lifeline-mobile-stat-tracker-icon", "Lifeline Mobile Stat Tracker Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.879883f), ImVec2(0.557617f, 0.932617f) },
    { "light-rounds", "Light Rounds.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.879883f), ImVec2(0.620117f, 0.932617f) },
    { "loba-icon", "Loba Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.879883f), ImVec2(0.682617f, 0.932617f) },
    { "loba-mobile-stat-tracker-icon", "Loba Mobile Stat Tracker Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.879883f), ImVec2(0.745117f, 0.932617f) },
    { "locked-and-loaded-icon", "Locked and Loaded Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.879883f), ImVec2(0.807617f, 0.932617f) },
    { "longbow-dmr-icon", "Longbow DMR Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.879883f), ImVec2(0.870117f, 0.932617f) },
    { "loot-spawner", "Loot Spawner.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.879883f), ImVec2(0.932617f, 0.932617f) },
    { "lost-treasures-icon", "Lost Treasures Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.879883f), ImVec2(0.995117f, 0.932617f) },
    { "low-profile", "Low Profile.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.942383f), ImVec2(0.057617f, 0.995117f) },
    { "ltm-icon", "LTM Icon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.942383f), ImVec2(0.120117f, 0.995117f) },
    { "m600-spitfire-icon", "M600 Spitfire Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.942383f), ImVec2(0.182617f, 0.995117f) },
    { "mad-maggie-icon", "Mad Maggie Icon.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.942383f), ImVec2(0.245117f, 0.995117f) },
    { "mapicon-care-package", "MapIcon Care Package.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.942383f), ImVec2(0.307617f, 0.995117f) },
    { "mapicon-dropship", "MapIcon Dropship.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.942383f), ImVec2(0.370117f, 0.995117f) },
    { "mapicon-interception-pylon", "MapIcon Interception Pylon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.942383f), ImVec2(0.432617f, 0.995117f) },
    { "mapicon-nessy", "MapIcon Nessy.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.942383f), ImVec2(0.495117f, 0.995117f) },
    { "mapicon-player", "MapIcon Player.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.942383f), ImVec2(0.557617f, 0.995117f) },
    { "mapicon-prowler-nest", "MapIcon Prowler Nest.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.942383f), ImVec2(0.620117f, 0.995117f) },
    { "mapicon-respawn-beacon", "MapIcon Respawn Beacon.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.942383f), ImVec2(0.682617f, 0.995117f) },
    { "mapicon-ring-console", "MapIcon Ring Console.svg", 0, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.942383f), ImVec2(0.745117f, 0.995117f) },
    { "mapicon-shadowfall-killer", "MapIcon Shadowfall Killer.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.942383f), ImVec2(0.807617f, 0.995117f) },
    { "mapicon-shadowfall-legend-death", "MapIcon Shadowfall Legend Death.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.942383f), ImVec2(0.870117f, 0.995117f) },
    { "mapicon-shadowfall-shadow-death", "MapIcon Shadowfall Shadow Death.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.942383f), ImVec2(0.932617f, 0.995117f) },
    { "mapicon-spider-nest", "MapIcon Spider Nest.png", 0, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.942383f), ImVec2(0.995117f, 0.995117f) },
    { "mapicon-supply-ship-landing-spot", "MapIcon Supply Ship Landing Spot.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.004883f), ImVec2(0.057617f, 0.057617f) },
    { "mapicon-supply-ship", "MapIcon Supply Ship.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.004883f), ImVec2(0.120117f, 0.057617f) },
    { "mapicon-survey-beacon", "MapIcon Survey Beacon.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.004883f), ImVec2(0.182617f, 0.057617f) },
    { "mapicon-train", "MapIcon Train.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.004883f), ImVec2(0.245117f, 0.057617f) },
    { "mapicon-trident-spawn", "MapIcon Trident Spawn.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.004883f), ImVec2(0.307617f, 0.057617f) },
    { "mapicon-vault", "MapIcon Vault.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.004883f), ImVec2(0.370117f, 0.057617f) },
    { "marked-for-death", "Marked for Death.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.004883f), ImVec2(0.432617f, 0.057617f) },
    { "mastiff-shotgun-icon", "Mastiff Shotgun Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.004883f), ImVec2(0.495117f, 0.057617f) },
    { "materials-blue", "Materials blue.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.004883f), ImVec2(0.557617f, 0.057617f) },
    { "med-kit", "Med Kit.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.004883f), ImVec2(0.620117f, 0.057617f) },
    { "mercenary-syndicate", "Mercenary Syndicate.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.004883f), ImVec2(0.682617f, 0.057617f) },
    { "mirage-icon", "Mirage Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.004883f), ImVec2(0.745117f, 0.057617f) },
    { "mirage-mobile-icon", "Mirage Mobile Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.004883f), ImVec2(0.807617f, 0.057617f) },
    { "mirage-mobile-stat-tracker-icon", "Mirage Mobile Stat Tracker Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.004883f), ImVec2(0.870117f, 0.057617f) },
    { "missile-swarm", "Missile Swarm.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.004883f), ImVec2(0.932617f, 0.057617f) },
    { "mobile-minigun-sheila", "Mobile Minigun -Sheila-.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.004883f), ImVec2(0.995117f, 0.057617f) },
    { "mobile-respawn-beacon", "Mobile Respawn Beacon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.067383f), ImVec2(0.057617f, 0.120117f) },
    { "mobile-shield", "Mobile Shield.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.067383f), ImVec2(0.120117f, 0.120117f) },
    { "mobile-team-deathmatch", "Mobile Team Deathmatch.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.067383f), ImVec2(0.182617f, 0.120117f) },
    { "modded-loader", "Modded Loader.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.067383f), ImVec2(0.245117f, 0.120117f) },
    { "monsters-within-icon", "Monsters Within Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.067383f), ImVec2(0.307617f, 0.120117f) },
    { "mozambique-shotgun-icon", "Mozambique Shotgun Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.067383f), ImVec2(0.370117f, 0.120117f) },
    { "mrvn-arm", "MRVN Arm.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.067383f), ImVec2(0.432617f, 0.120117f) },
    { "music-pack-icon", "Music Pack Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.067383f), ImVec2(0.495117f, 0.120117f) },
    { "mythic-arrows", "Mythic Arrows.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.067383f), ImVec2(0.557617f, 0.120117f) },
    { "mythic-energy-ammo", "Mythic Energy Ammo.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.067383f), ImVec2(0.620117f, 0.120117f) },
    { "mythic-heavy-rounds", "Mythic Heavy Rounds.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.067383f), ImVec2(0.682617f, 0.120117f) },
    { "mythic-light-rounds", "Mythic Light Rounds.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.067383f), ImVec2(0.745117f, 0.120117f) },
    { "mythic-shotgun-shells", "Mythic Shotgun Shells.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.067383f), ImVec2(0.807617f, 0.120117f) },
    { "mythic-sniper-ammo", "Mythic Sniper Ammo.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.067383f), ImVec2(0.870117f, 0.120117f) },
    { "mythic-throwing-knife-ammo", "Mythic Throwing Knife Ammo.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.067383f), ImVec2(0.932617f, 0.120117f) },
    { "national-alliance-of-boreas", "National Alliance of Boreas.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.067383f), ImVec2(0.995117f, 0.120117f) },
    { "nemesis-burst-ar-icon", "Nemesis Burst AR Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.129883f), ImVec2(0.057617f, 0.182617f) },
    { "neon-network-icon", "Neon Network Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.129883f), ImVec2(0.120117f, 0.182617f) },
    { "neon-network", "Neon Network.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.129883f), ImVec2(0.182617f, 0.182617f) },
    { "neurolink", "Neurolink.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.129883f), ImVec2(0.245117f, 0.182617f) },
    { "newcastle-icon", "Newcastle Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.129883f), ImVec2(0.307617f, 0.182617f) },
    { "nintendo-switch-icon", "Nintendo Switch Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.129883f), ImVec2(0.370117f, 0.182617f) },
    { "now-you-see-me", "Now You See Me....svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.129883f), ImVec2(0.432617f, 0.182617f) },
    { "nox-gas-grenade", "Nox Gas Grenade.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.129883f), ImVec2(0.495117f, 0.182617f) },
    { "nox-gas-trap", "Nox Gas Trap.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.129883f), ImVec2(0.557617f, 0.182617f) },
    { "nox-vision", "Nox Vision.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.129883f), ImVec2(0.620117f, 0.182617f) },
    { "octane-icon", "Octane Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.129883f), ImVec2(0.682617f, 0.182617f) },
    { "octane-mobile-stat-tracker-icon", "Octane Mobile Stat Tracker Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.129883f), ImVec2(0.745117f, 0.182617f) },
    { "offensive", "Offensive.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.129883f), ImVec2(0.807617f, 0.182617f) },
    { "optics-slot", "Optics Slot.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.129883f), ImVec2(0.870117f, 0.182617f) },
    { "origin-icon", "Origin Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.129883f), ImVec2(0.932617f, 0.182617f) },
    { "p2020-icon", "P2020 Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.129883f), ImVec2(0.995117f, 0.182617f) },
    { "panic-button", "Panic Button.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.192383f), ImVec2(0.057617f, 0.245117f) },
    { "paradinha-arsenal-icon", "Paradinha Arsenal Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.192383f), ImVec2(0.120117f, 0.245117f) },
    { "participation-icon", "Participation Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.192383f), ImVec2(0.182617f, 0.245117f) },
    { "pathfinder-icon", "Pathfinder Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.192383f), ImVec2(0.245117f, 0.245117f) },
    { "pathfinder-mobile-icon", "Pathfinder Mobile Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.192383f), ImVec2(0.307617f, 0.245117f) },
    { "pathfinder-mobile-stat-tracker-icon", "Pathfinder Mobile Stat Tracker Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.192383f), ImVec2(0.370117f, 0.245117f) },
    { "peacekeeper-icon", "Peacekeeper Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.192383f), ImVec2(0.432617f, 0.245117f) },
    { "perimeter-security", "Perimeter Security.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.192383f), ImVec2(0.495117f, 0.245117f) },
    { "perk-coin-bangalore", "Perk Coin Bangalore.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.192383f), ImVec2(0.557617f, 0.245117f) },
    { "perk-coin-bloodhound", "Perk Coin Bloodhound.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.192383f), ImVec2(0.620117f, 0.245117f) },
    { "perk-coin-caustic", "Perk Coin Caustic.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.192383f), ImVec2(0.682617f, 0.245117f) },
    { "perk-coin-fade", "Perk Coin Fade.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.192383f), ImVec2(0.745117f, 0.245117f) },
    { "perk-coin-gibraltar", "Perk Coin Gibraltar.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.192383f), ImVec2(0.807617f, 0.245117f) },
    { "perk-coin-lifeline", "Perk Coin Lifeline.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.192383f), ImVec2(0.870117f, 0.245117f) },
    { "perk-coin-loba", "Perk Coin Loba.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.192383f), ImVec2(0.932617f, 0.245117f) },
    { "perk-coin-mirage", "Perk Coin Mirage.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.192383f), ImVec2(0.995117f, 0.245117f) },
    { "perk-coin-octane", "Perk Coin Octane.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.254883f), ImVec2(0.057617f, 0.307617f) },
    { "perk-coin-pathfinder", "Perk Coin Pathfinder.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.254883f), ImVec2(0.120117f, 0.307617f) },
    { "perk-coin-wraith", "Perk Coin Wraith.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.254883f), ImVec2(0.182617f, 0.307617f) },
    { "phase-breach", "Phase Breach.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.254883f), ImVec2(0.245117f, 0.307617f) },
    { "phase-chamber", "Phase Chamber.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.254883f), ImVec2(0.307617f, 0.307617f) },
    { "phoenix-kit", "Phoenix Kit.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.254883f), ImVec2(0.370117f, 0.307617f) },
    { "piercing-spikes", "Piercing Spikes.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.254883f), ImVec2(0.432617f, 0.307617f) },
    { "ping-attacking-here", "Ping Attacking Here.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.254883f), ImVec2(0.495117f, 0.307617f) },
    { "ping-basic", "Ping Basic.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.254883f), ImVec2(0.557617f, 0.307617f) },
    { "ping-deathbox-banner", "Ping Deathbox Banner.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.254883f), ImVec2(0.620117f, 0.307617f) },
    { "ping-decoy-shot", "Ping Decoy Shot.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.254883f), ImVec2(0.682617f, 0.307617f) },
    { "ping-defending-here", "Ping Defending Here.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.254883f), ImVec2(0.745117f, 0.307617f) },
    { "ping-dibs", "Ping Dibs.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.254883f), ImVec2(0.807617f, 0.307617f) },
    { "ping-enemy-here", "Ping Enemy Here.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.254883f), ImVec2(0.870117f, 0.307617f) },
    { "ping-fence-crossed", "Ping Fence Crossed.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.254883f), ImVec2(0.932617f, 0.307617f) },
    { "ping-going-here", "Ping Going Here.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.254883f), ImVec2(0.995117f, 0.307617f) },
    { "ping-help", "Ping Help.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.317383f), ImVec2(0.057617f, 0.370117f) },
    { "ping-i-can-t", "Ping I Can't.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.317383f), ImVec2(0.120117f, 0.370117f) },
    { "ping-join", "Ping Join.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.317383f), ImVec2(0.182617f, 0.370117f) },
    { "ping-looting-here", "Ping Looting Here.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.317383f), ImVec2(0.245117f, 0.370117f) },
    { "ping-nice", "Ping Nice.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.317383f), ImVec2(0.307617f, 0.370117f) },
    { "ping-no", "Ping No.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.317383f), ImVec2(0.370117f, 0.370117f) },
    { "ping-ok", "Ping Ok.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.317383f), ImVec2(0.432617f, 0.370117f) },
    { "ping-recent-battle", "Ping Recent Battle.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.317383f), ImVec2(0.495117f, 0.370117f) },
    { "ping-someones-been-here", "Ping Someones Been Here.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.317383f), ImVec2(0.557617f, 0.370117f) },
    { "ping-watching-here", "Ping Watching Here.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.317383f), ImVec2(0.620117f, 0.370117f) },
    { "playstation-icon", "Playstation Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.317383f), ImVec2(0.682617f, 0.370117f) },
    { "precision-choke", "Precision Choke.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.317383f), ImVec2(0.745117f, 0.370117f) },
    { "prime-gaming", "Prime Gaming.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.317383f), ImVec2(0.807617f, 0.370117f) },
    { "proteus-icon", "Proteus Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.317383f), ImVec2(0.870117f, 0.370117f) },
    { "prowler-burst-pdw-icon", "Prowler Burst PDW Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.317383f), ImVec2(0.932617f, 0.370117f) },
    { "ps4-trophy-bronze-icon", "PS4 Trophy Bronze Icon.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.317383f), ImVec2(0.995117f, 0.370117f) },
    { "ps4-trophy-gold-icon", "PS4 Trophy Gold Icon.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.379883f), ImVec2(0.057617f, 0.432617f) },
    { "ps4-trophy-silver-icon", "PS4 Trophy Silver Icon.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.379883f), ImVec2(0.120117f, 0.432617f) },
    { "psplus-logo", "PSPlus Logo.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.379883f), ImVec2(0.182617f, 0.432617f) },
    { "psyche-out", "Psyche Out.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.379883f), ImVec2(0.245117f, 0.432617f) },
    { "quest-icon", "Quest Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.379883f), ImVec2(0.307617f, 0.432617f) },
    { "quickdraw-holster", "Quickdraw Holster.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.379883f), ImVec2(0.370117f, 0.432617f) },
    { "quip-icon", "Quip Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.379883f), ImVec2(0.432617f, 0.432617f) },
    { "r-301-carbine-icon", "R-301 Carbine Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.379883f), ImVec2(0.495117f, 0.432617f) },
    { "r-99-smg-icon", "R-99 SMG Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.379883f), ImVec2(0.557617f, 0.432617f) },
    { "radiant-transfer", "Radiant Transfer.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.379883f), ImVec2(0.620117f, 0.432617f) },
    { "raiders-icon", "Raiders Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.379883f), ImVec2(0.682617f, 0.432617f) },
    { "rampage-lmg-icon", "Rampage LMG Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.379883f), ImVec2(0.745117f, 0.432617f) },
    { "rampart-icon", "Rampart Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.379883f), ImVec2(0.807617f, 0.432617f) },
    { "rampart-symbol-icon", "Rampart Symbol Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.379883f), ImVec2(0.870117f, 0.432617f) },
    { "ranked-tier1-bronze", "Ranked Tier1 Bronze.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.379883f), ImVec2(0.932617f, 0.432617f) },
    { "ranked-tier2-silver", "Ranked Tier2 Silver.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.379883f), ImVec2(0.995117f, 0.432617f) },
    { "ranked-tier3-gold", "Ranked Tier3 Gold.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.442383f), ImVec2(0.057617f, 0.495117f) },
    { "ranked-tier4-platinum", "Ranked Tier4 Platinum.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.442383f), ImVec2(0.120117f, 0.495117f) },
    { "ranked-tier5-diamond", "Ranked Tier5 Diamond.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.442383f), ImVec2(0.182617f, 0.495117f) },
    { "ranked-tier6-master", "Ranked Tier6 Master.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.442383f), ImVec2(0.245117f, 0.495117f) },
    { "ranked-tier7-apex-predator", "Ranked Tier7 Apex Predator.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.442383f), ImVec2(0.307617f, 0.495117f) },
    { "re-45-auto-icon", "RE-45 Auto Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.442383f), ImVec2(0.370117f, 0.495117f) },
    { "recon-class", "Recon Class.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.442383f), ImVec2(0.432617f, 0.495117f) },
    { "recon-old", "Recon old.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.442383f), ImVec2(0.495117f, 0.495117f) },
    { "reduced-tactical-recharge", "Reduced Tactical Recharge.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.442383f), ImVec2(0.557617f, 0.495117f) },
    { "reduced-ultimate-recharge", "Reduced Ultimate Recharge.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.442383f), ImVec2(0.620117f, 0.495117f) },
    { "replicator-icon", "Replicator Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.442383f), ImVec2(0.682617f, 0.495117f) },
    { "retrieve-the-wounded", "Retrieve the Wounded.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.442383f), ImVec2(0.745117f, 0.495117f) },
    { "revenant-army-icon", "Revenant Army Icon.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.442383f), ImVec2(0.807617f, 0.495117f) },
    { "revenant-icon", "Revenant Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.442383f), ImVec2(0.870117f, 0.495117f) },
    { "revenant-thematic-pack-icon", "Revenant Thematic Pack Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.442383f), ImVec2(0.932617f, 0.495117f) },
    { "rhapsody-icon", "Rhapsody Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.442383f), ImVec2(0.995117f, 0.495117f) },
    { "ring-fury-icon", "Ring Fury Icon.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.504883f), ImVec2(0.057617f, 0.557617f) },
    { "riot-drill", "Riot Drill.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.504883f), ImVec2(0.120117f, 0.557617f) },
    { "rolling-thunder", "Rolling Thunder.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.504883f), ImVec2(0.182617f, 0.557617f) },
    { "rowdy-s-rave", "Rowdy's Rave.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.504883f), ImVec2(0.245117f, 0.557617f) },
    { "sale-icon", "Sale Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.504883f), ImVec2(0.307617f, 0.557617f) },
    { "salvo-icon", "Salvo Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.504883f), ImVec2(0.370117f, 0.557617f) },
    { "salvo-mc-icon", "Salvo MC Icon.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.504883f), ImVec2(0.432617f, 0.557617f) },
    { "saras-icon", "SARAS Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.504883f), ImVec2(0.495117f, 0.557617f) },
    { "savior-s-speed", "Savior's Speed.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.504883f), ImVec2(0.557617f, 0.557617f) },
    { "season-1-mobile", "Season 1 (Mobile).svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.504883f), ImVec2(0.620117f, 0.557617f) },
    { "season-1-icon", "Season 1 Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.504883f), ImVec2(0.682617f, 0.557617f) },
    { "season-1", "Season 1.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.504883f), ImVec2(0.745117f, 0.557617f) },
    { "season-10-icon", "Season 10 Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.504883f), ImVec2(0.807617f, 0.557617f) },
    { "season-10", "Season 10.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.504883f), ImVec2(0.870117f, 0.557617f) },
    { "season-11", "Season 11.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.504883f), ImVec2(0.932617f, 0.557617f) },
    { "season-12-imc", "Season 12 IMC.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.504883f), ImVec2(0.995117f, 0.557617f) },
    { "season-12", "Season 12.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.567383f), ImVec2(0.057617f, 0.620117f) },
    { "season-13-icon", "Season 13 Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.567383f), ImVec2(0.120117f, 0.620117f) },
    { "season-13", "Season 13.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.567383f), ImVec2(0.182617f, 0.620117f) },
    { "season-14", "Season 14.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.567383f), ImVec2(0.245117f, 0.620117f) },
    { "season-15", "Season 15.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.567383f), ImVec2(0.307617f, 0.620117f) },
    { "season-16", "Season 16.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.567383f), ImVec2(0.370117f, 0.620117f) },
    { "season-17", "Season 17.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.567383f), ImVec2(0.432617f, 0.620117f) },
    { "season-18", "Season 18.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.567383f), ImVec2(0.495117f, 0.620117f) },
    { "season-19", "Season 19.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.567383f), ImVec2(0.557617f, 0.620117f) },
    { "season-2-mobile", "Season 2 (Mobile).svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.567383f), ImVec2(0.620117f, 0.620117f) },
    { "season-2-icon", "Season 2 Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.567383f), ImVec2(0.682617f, 0.620117f) },
    { "season-2", "Season 2.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.567383f), ImVec2(0.745117f, 0.620117f) },
    { "season-3-mobile", "Season 3 (Mobile).svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.567383f), ImVec2(0.807617f, 0.620117f) },
    { "season-3-icon", "Season 3 Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.567383f), ImVec2(0.870117f, 0.620117f) },
    { "season-3", "Season 3.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.567383f), ImVec2(0.932617f, 0.620117f) },
    { "season-4-mobile", "Season 4 (Mobile).svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.567383f), ImVec2(0.995117f, 0.620117f) },
    { "season-4-icon", "Season 4 Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.629883f), ImVec2(0.057617f, 0.682617f) },
    { "season-4", "Season 4.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.629883f), ImVec2(0.120117f, 0.682617f) },
    { "season-5", "Season 5.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.629883f), ImVec2(0.182617f, 0.682617f) },
    { "season-6-icon", "Season 6 Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.629883f), ImVec2(0.245117f, 0.682617f) },
    { "season-6", "Season 6.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.629883f), ImVec2(0.307617f, 0.682617f) },
    { "season-7-icon", "Season 7 Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.629883f), ImVec2(0.370117f, 0.682617f) },
    { "season-7", "Season 7.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.629883f), ImVec2(0.432617f, 0.682617f) },
    { "season-8-icon", "Season 8 Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.629883f), ImVec2(0.495117f, 0.682617f) },
    { "season-8", "Season 8.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.629883f), ImVec2(0.557617f, 0.682617f) },
    { "season-9", "Season 9.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.629883f), ImVec2(0.620117f, 0.682617f) },
    { "seasonal-currency-s1-5", "Seasonal Currency S1.5.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.629883f), ImVec2(0.682617f, 0.682617f) },
    { "seasonal-currency-s1", "Seasonal Currency S1.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.629883f), ImVec2(0.745117f, 0.682617f) },
    { "seasonal-currency-s2-5", "Seasonal Currency S2.5.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.629883f), ImVec2(0.807617f, 0.682617f) },
    { "seasonal-currency-s2", "Seasonal Currency S2.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.629883f), ImVec2(0.870117f, 0.682617f) },
    { "seasonal-currency-s3-5", "Seasonal Currency S3.5.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.629883f), ImVec2(0.932617f, 0.682617f) },
    { "seasonal-currency-s3", "Seasonal Currency S3.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.629883f), ImVec2(0.995117f, 0.682617f) },
    { "seasonal-currency-s4", "Seasonal Currency S4.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.692383f), ImVec2(0.057617f, 0.745117f) },
    { "second-chance-icon", "Second Chance Icon.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.692383f), ImVec2(0.120117f, 0.745117f) },
    { "seer-icon", "Seer Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.692383f), ImVec2(0.182617f, 0.745117f) },
    { "selectfire-receiver", "Selectfire Receiver.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.692383f), ImVec2(0.245117f, 0.745117f) },
    { "sentinel-icon", "Sentinel Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.692383f), ImVec2(0.307617f, 0.745117f) },
    { "shadow-pounce", "Shadow Pounce.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.692383f), ImVec2(0.370117f, 0.745117f) },
    { "shadowfall-icon", "Shadowfall Icon.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.692383f), ImVec2(0.432617f, 0.745117f) },
    { "shatter-caps-active-icon", "Shatter Caps Active Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.692383f), ImVec2(0.495117f, 0.745117f) },
    { "shatter-caps", "Shatter Caps.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.692383f), ImVec2(0.557617f, 0.745117f) },
    { "sheila-icon", "SHEILA Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.692383f), ImVec2(0.620117f, 0.745117f) },
    { "shield-battery", "Shield Battery.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.692383f), ImVec2(0.682617f, 0.745117f) },
    { "shield-cell", "Shield Cell.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.692383f), ImVec2(0.745117f, 0.745117f) },
    { "shotgun-bolt", "Shotgun Bolt.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.692383f), ImVec2(0.807617f, 0.745117f) },
    { "shotgun-shells", "Shotgun Shells.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.692383f), ImVec2(0.870117f, 0.745117f) },
    { "silence", "Silence.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.692383f), ImVec2(0.932617f, 0.745117f) },
    { "silva-pharmaceuticals-icon", "Silva Pharmaceuticals Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.692383f), ImVec2(0.995117f, 0.745117f) },
    { "single-fire-icon", "Single-fire Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.754883f), ImVec2(0.057617f, 0.807617f) },
    { "siwhan-industries", "Siwhan Industries.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.754883f), ImVec2(0.120117f, 0.807617f) },
    { "skin-icon", "Skin Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.754883f), ImVec2(0.182617f, 0.807617f) },
    { "skirmisher-class", "Skirmisher Class.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.754883f), ImVec2(0.245117f, 0.807617f) },
    { "skullpiercer-rifling", "Skullpiercer Rifling.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.754883f), ImVec2(0.307617f, 0.807617f) },
    { "skydive-emote-icon", "Skydive Emote Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.754883f), ImVec2(0.370117f, 0.807617f) },
    { "skyward-dive", "Skyward Dive.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.754883f), ImVec2(0.432617f, 0.807617f) },
    { "sling", "Sling.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.754883f), ImVec2(0.495117f, 0.807617f) },
    { "slipstream", "Slipstream.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.754883f), ImVec2(0.557617f, 0.807617f) },
    { "smoke-launcher", "Smoke Launcher.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.754883f), ImVec2(0.620117f, 0.807617f) },
    { "sniper-ammo-amped", "Sniper Ammo Amped.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.754883f), ImVec2(0.682617f, 0.807617f) },
    { "sniper-ammo", "Sniper Ammo.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.754883f), ImVec2(0.745117f, 0.807617f) },
    { "sniper-stock", "Sniper Stock.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.754883f), ImVec2(0.807617f, 0.807617f) },
    { "sniper-s-mark", "Sniper's Mark.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.754883f), ImVec2(0.870117f, 0.807617f) },
    { "spacewalk", "Spacewalk.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.754883f), ImVec2(0.932617f, 0.807617f) },
    { "spark-of-genius", "Spark of Genius.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.754883f), ImVec2(0.995117f, 0.807617f) },
    { "spectator-icon", "Spectator Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.817383f), ImVec2(0.057617f, 0.870117f) },
    { "spellbound-icon", "Spellbound Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.817383f), ImVec2(0.120117f, 0.870117f) },
    { "spellbound", "Spellbound.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.817383f), ImVec2(0.182617f, 0.870117f) },
    { "splatter-rounds", "Splatter Rounds.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.817383f), ImVec2(0.245117f, 0.870117f) },
    { "spotter-s-lens", "Spotter's Lens.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.817383f), ImVec2(0.307617f, 0.870117f) },
    { "stalker", "Stalker.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.817383f), ImVec2(0.370117f, 0.870117f) },
    { "standard-stock", "Standard Stock.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.817383f), ImVec2(0.432617f, 0.870117f) },
    { "starter-pack-icon", "Starter Pack Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.817383f), ImVec2(0.495117f, 0.870117f) },
    { "stat-tracker-icon", "Stat Tracker Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.817383f), ImVec2(0.557617f, 0.870117f) },
    { "steam-icon", "Steam Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.817383f), ImVec2(0.620117f, 0.870117f) },
    { "steam-logo", "Steam Logo.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.817383f), ImVec2(0.682617f, 0.870117f) },
    { "sticker-icon", "Sticker Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.817383f), ImVec2(0.745117f, 0.870117f) },
    { "stim", "Stim.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.817383f), ImVec2(0.807617f, 0.870117f) },
    { "sun-squad-icon", "Sun Squad Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.817383f), ImVec2(0.870117f, 0.870117f) },
    { "sun-squad", "Sun Squad.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.817383f), ImVec2(0.932617f, 0.870117f) },
    { "support-class", "Support Class.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.817383f), ImVec2(0.995117f, 0.870117f) },
    { "support-old", "Support old.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.879883f), ImVec2(0.057617f, 0.932617f) },
    { "surveillance-drone", "Surveillance Drone.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.879883f), ImVec2(0.120117f, 0.932617f) },
    { "swift-mend", "Swift Mend.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.879883f), ImVec2(0.182617f, 0.932617f) },
    { "syndicate-corps-icon", "Syndicate Corps Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.879883f), ImVec2(0.245117f, 0.932617f) },
    { "syndicate-gold", "Syndicate Gold.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.879883f), ImVec2(0.307617f, 0.932617f) },
    { "syringe", "Syringe.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.879883f), ImVec2(0.370117f, 0.932617f) },
    { "system-override-icon", "System Override Icon.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.879883f), ImVec2(0.432617f, 0.932617f) },
    { "tempest", "Tempest.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.879883f), ImVec2(0.495117f, 0.932617f) },
    { "the-motherlode", "The Motherlode.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.879883f), ImVec2(0.557617f, 0.932617f) },
    { "the-old-ways-icon", "The Old Ways Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.879883f), ImVec2(0.620117f, 0.932617f) },
    { "the-sisters", "The Sisters.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.879883f), ImVec2(0.682617f, 0.932617f) },
    { "thermite-grenade", "Thermite Grenade.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.879883f), ImVec2(0.745117f, 0.932617f) },
    { "threat-level-icon", "Threat Level Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.879883f), ImVec2(0.807617f, 0.932617f) },
    { "threat-level", "Threat Level.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.879883f), ImVec2(0.870117f, 0.932617f) },
    { "thrillseekers-icon", "Thrillseekers Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.879883f), ImVec2(0.932617f, 0.932617f) },
    { "throwing-knife-icon", "Throwing Knife Icon.svg", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.879883f), ImVec2(0.995117f, 0.932617f) },
    { "tracker-icon-ability-placed", "Tracker Icon Ability Placed.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.942383f), ImVec2(0.057617f, 0.995117f) },
    { "tracker-icon-blood", "Tracker Icon Blood.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.942383f), ImVec2(0.120117f, 0.995117f) },
    { "tracker-icon-door-destroyed", "Tracker Icon Door Destroyed.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.942383f), ImVec2(0.182617f, 0.995117f) },
    { "tracker-icon-door-used", "Tracker Icon Door Used.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.942383f), ImVec2(0.245117f, 0.995117f) },
    { "tracker-icon-empty-shells", "Tracker Icon Empty Shells.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.942383f), ImVec2(0.307617f, 0.995117f) },
    { "tracker-icon-entered-phase", "Tracker Icon Entered Phase.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.942383f), ImVec2(0.370117f, 0.995117f) },
    { "tracker-icon-footprint", "Tracker Icon Footprint.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.942383f), ImVec2(0.432617f, 0.995117f) },
    { "tracker-icon-gas-residue", "Tracker Icon Gas Residue.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.942383f), ImVec2(0.495117f, 0.995117f) },
    { "tracker-icon-grenade-pin", "Tracker Icon Grenade Pin.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.942383f), ImVec2(0.557617f, 0.995117f) },
    { "tracker-icon-interception-pylon-destroyed", "Tracker Icon Interception Pylon Destroyed.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.942383f), ImVec2(0.620117f, 0.995117f) },
    { "tracker-icon-item-discarded", "Tracker Icon Item Discarded.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.942383f), ImVec2(0.682617f, 0.995117f) },
    { "tracker-icon-item-swapped", "Tracker Icon Item Swapped.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.942383f), ImVec2(0.745117f, 0.995117f) },
    { "tracker-icon-killed-by-weapon", "Tracker Icon Killed by WEAPON.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.942383f), ImVec2(0.807617f, 0.995117f) },
    { "tracker-icon-killer-with-weapon", "Tracker Icon Killer With WEAPON.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.942383f), ImVec2(0.870117f, 0.995117f) },
    { "tracker-icon-landed", "Tracker Icon Landed.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.942383f), ImVec2(0.932617f, 0.995117f) },
    { "tracker-icon-left-phase", "Tracker Icon Left Phase.png", 1, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.942383f), ImVec2(0.995117f, 0.995117f) },
    { "tracker-icon-left-zipline", "Tracker Icon Left Zipline.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.004883f), ImVec2(0.057617f, 0.057617f) },
    { "tracker-icon-loot-bin-opened", "Tracker Icon Loot Bin Opened.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.004883f), ImVec2(0.120117f, 0.057617f) },
    { "tracker-icon-mantled", "Tracker Icon Mantled.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.004883f), ImVec2(0.182617f, 0.057617f) },
    { "tracker-icon-perimeter-security-destroyed", "Tracker Icon Perimeter Security Destroyed.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.004883f), ImVec2(0.245117f, 0.057617f) },
    { "tracker-icon-picked-up-item", "Tracker Icon Picked Up ITEM.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.004883f), ImVec2(0.307617f, 0.057617f) },
    { "tracker-icon-pod-opened", "Tracker Icon Pod Opened.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.004883f), ImVec2(0.370117f, 0.057617f) },
    { "tracker-icon-severe-bleeding", "Tracker Icon Severe Bleeding.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.004883f), ImVec2(0.432617f, 0.057617f) },
    { "tracker-icon-slide-mark", "Tracker Icon Slide Mark.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.004883f), ImVec2(0.495117f, 0.057617f) },
    { "tracker-icon-smoke-residue", "Tracker Icon Smoke Residue.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.004883f), ImVec2(0.557617f, 0.057617f) },
    { "tracker-icon-weapon-reloaded", "Tracker Icon Weapon Reloaded.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.004883f), ImVec2(0.620117f, 0.057617f) },
    { "tracker-icon-wound-bandaged", "Tracker Icon Wound Bandaged.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.004883f), ImVec2(0.682617f, 0.057617f) },
    { "tracker-icon-ziplined", "Tracker Icon Ziplined.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.004883f), ImVec2(0.745117f, 0.057617f) },
    { "tracker", "Tracker.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.004883f), ImVec2(0.807617f, 0.057617f) },
    { "transition-icon", "Transition Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.004883f), ImVec2(0.870117f, 0.057617f) },
    { "treasure-pack-icon", "Treasure Pack Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.004883f), ImVec2(0.932617f, 0.057617f) },
    { "trials-icon", "Trials Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.004883f), ImVec2(0.995117f, 0.057617f) },
    { "trident-spawn-location-icon", "Trident Spawn Location Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.067383f), ImVec2(0.057617f, 0.120117f) },
    { "triple-take-icon", "Triple Take Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.067383f), ImVec2(0.120117f, 0.120117f) },
    { "turbocharger", "Turbocharger.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.067383f), ImVec2(0.182617f, 0.120117f) },
    { "twitch-logo", "Twitch Logo.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.067383f), ImVec2(0.245117f, 0.120117f) },
    { "ultimate-accelerant", "Ultimate Accelerant.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.067383f), ImVec2(0.307617f, 0.120117f) },
    { "ultra-zones-icon", "Ultra Zones Icon.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.067383f), ImVec2(0.370117f, 0.120117f) },
    { "unshackled-icon", "Unshackled Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.067383f), ImVec2(0.432617f, 0.120117f) },
    { "uprising-icon", "Uprising Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.067383f), ImVec2(0.495117f, 0.120117f) },
    { "uprising", "Uprising.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.067383f), ImVec2(0.557617f, 0.120117f) },
    { "valentines-event-icon", "Valentines Event Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.067383f), ImVec2(0.620117f, 0.120117f) },
    { "valkyrie-icon", "Valkyrie Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.067383f), ImVec2(0.682617f, 0.120117f) },
    { "vantage-icon", "Vantage Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.067383f), ImVec2(0.745117f, 0.120117f) },
    { "vault-key", "Vault Key.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.067383f), ImVec2(0.807617f, 0.120117f) },
    { "veiled-icon", "Veiled Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.067383f), ImVec2(0.870117f, 0.120117f) },
    { "veiled", "Veiled.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.067383f), ImVec2(0.932617f, 0.120117f) },
    { "vinson-dynamics-icon", "Vinson Dynamics Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.067383f), ImVec2(0.995117f, 0.120117f) },
    { "vk-47-flatline-icon", "VK-47 Flatline Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.129883f), ImVec2(0.057617f, 0.182617f) },
    { "voices-from-the-void", "Voices from the Void.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.129883f), ImVec2(0.120117f, 0.182617f) },
    { "voidwalker-icon", "Voidwalker Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.129883f), ImVec2(0.182617f, 0.182617f) },
    { "volt-smg-icon", "Volt SMG Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.129883f), ImVec2(0.245117f, 0.182617f) },
    { "vtol-jets", "VTOL Jets.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.129883f), ImVec2(0.307617f, 0.182617f) },
    { "war-games-event-icon", "War Games Event Icon.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.129883f), ImVec2(0.370117f, 0.182617f) },
    { "war-games-icon", "War Games Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.129883f), ImVec2(0.432617f, 0.182617f) },
    { "warlord-s-ire", "Warlord's Ire.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.129883f), ImVec2(0.495117f, 0.182617f) },
    { "warriors-pack-icon", "Warriors Pack Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.129883f), ImVec2(0.557617f, 0.182617f) },
    { "warriors", "Warriors.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.129883f), ImVec2(0.620117f, 0.182617f) },
    { "wattson-icon", "Wattson Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.129883f), ImVec2(0.682617f, 0.182617f) },
    { "weapon-experience-icon", "Weapon Experience Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.692383f, 0.129883f), ImVec2(0.745117f, 0.182617f) },
    { "whistler", "Whistler.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.754883f, 0.129883f), ImVec2(0.807617f, 0.182617f) },
    { "wingman-icon", "Wingman Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.817383f, 0.129883f), ImVec2(0.870117f, 0.182617f) },
    { "winter-express-icon", "Winter Express Icon.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.879883f, 0.129883f), ImVec2(0.932617f, 0.182617f) },
    { "wintertide", "Wintertide.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.942383f, 0.129883f), ImVec2(0.995117f, 0.182617f) },
    { "wonyeon", "Wonyeon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.004883f, 0.192383f), ImVec2(0.057617f, 0.245117f) },
    { "world-s-edge-mobile-icon", "World's Edge Mobile Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.067383f, 0.192383f), ImVec2(0.120117f, 0.245117f) },
    { "wraith-icon", "Wraith Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.129883f, 0.192383f), ImVec2(0.182617f, 0.245117f) },
    { "wraith-mobile-icon", "Wraith Mobile Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.192383f, 0.192383f), ImVec2(0.245117f, 0.245117f) },
    { "wraith-mobile-stat-tracker-icon", "Wraith Mobile Stat Tracker Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.254883f, 0.192383f), ImVec2(0.307617f, 0.245117f) },
    { "wraith-thematic-pack-icon", "Wraith Thematic Pack Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.317383f, 0.192383f), ImVec2(0.370117f, 0.245117f) },
    { "wrecking-ball", "Wrecking Ball.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.379883f, 0.192383f), ImVec2(0.432617f, 0.245117f) },
    { "xbox-gamerscore-icon", "XBox Gamerscore Icon.png", 2, ImVec2(108.0f, 108.0f), ImVec2(0.442383f, 0.192383f), ImVec2(0.495117f, 0.245117f) },
    { "xbox-icon", "Xbox Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.504883f, 0.192383f), ImVec2(0.557617f, 0.245117f) },
    { "xp-boost-icon", "XP Boost Icon.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.567383f, 0.192383f), ImVec2(0.620117f, 0.245117f) },
    { "zipline-gun", "Zipline Gun.svg", 2, ImVec2(108.0f, 108.0f), ImVec2(0.629883f, 0.192383f), ImVec2(0.682617f, 0.245117f) },
};

// Ability Icons
#define APEX_COLOR_ICON_AMPED_COVER 17
#define APEX_COLOR_ICON_ARC_SNARE 24
#define APEX_COLOR_ICON_ASSASSIN_S_INSTINCT 31
#define APEX_COLOR_ICON_BARRICADE 50
#define APEX_COLOR_ICON_BEAST_OF_THE_HUNT 55
#define APEX_COLOR_ICON_BLACK_HOLE 56
#define APEX_COLOR_ICON_BLACK_MARKET_BOUTIQUE 57
#define APEX_COLOR_ICON_BURGLAR_S_BEST_FRIEND 64
#define APEX_COLOR_ICON_CARE_PACKAGE 69
#define APEX_COLOR_ICON_CASTLE_WALL 70
#define APEX_COLOR_ICON_COMBAT_REVIVE 87
#define APEX_COLOR_ICON_D_O_C_HEAL_DRONE 100
#define APEX_COLOR_ICON_DARK_VEIL 103
#define APEX_COLOR_ICON_DEATH_TOTEM 108
#define APEX_COLOR_ICON_DEFENSIVE_BOMBARDMENT 109
#define APEX_COLOR_ICON_DIMENSIONAL_RIFT 113
#define APEX_COLOR_ICON_DOME_OF_PROTECTION 115
#define APEX_COLOR_ICON_DOUBLE_TIME 118
#define APEX_COLOR_ICON_DRONE_EMP 122
#define APEX_COLOR_ICON_ECHO_RELOCATION 126
#define APEX_COLOR_ICON_ENERGY_BARRICADE 131
#define APEX_COLOR_ICON_EXHIBIT 140
#define APEX_COLOR_ICON_EYE_FOR_QUALITY 146
#define APEX_COLOR_ICON_EYE_OF_THE_ALLFATHER 147
#define APEX_COLOR_ICON_FLASHBACK 156
#define APEX_COLOR_ICON_FOCUS_OF_ATTENTION 158
#define APEX_COLOR_ICON_FORGED_SHADOWS 159
#define APEX_COLOR_ICON_FORTIFIED 160
#define APEX_COLOR_ICON_GIFTED_EAR 172
#define APEX_COLOR_ICON_GRAPPLING_HOOK 175
#define APEX_COLOR_ICON_GRAVITY_LIFT 176
#define APEX_COLOR_ICON_GRENADIER 177
#define APEX_COLOR_ICON_GUN_SHIELD 180
#define APEX_COLOR_ICON_HEART_SEEKER 185
#define APEX_COLOR_ICON_HYPE_ANTHEM 197
#define APEX_COLOR_ICON_INSIDER_KNOWLEDGE 208
#define APEX_COLOR_ICON_INTERCEPTION_PYLON 209
#define APEX_COLOR_ICON_INTO_THE_VOID 211
#define APEX_COLOR_ICON_KNUCKLE_CLUSTER 219
#define APEX_COLOR_ICON_LAUNCH_PAD 225
#define APEX_COLOR_ICON_LIFE_OF_THE_PARTY 230
#define APEX_COLOR_ICON_LOOT_SPAWNER 238
#define APEX_COLOR_ICON_LOW_PROFILE 240
#define APEX_COLOR_ICON_MARKED_FOR_DEATH 262
#define APEX_COLOR_ICON_MISSILE_SWARM 270
#define APEX_COLOR_ICON_MOBILE_MINIGUN_SHEILA 271
#define APEX_COLOR_ICON_MOBILE_SHIELD 273
#define APEX_COLOR_ICON_MODDED_LOADER 275
#define APEX_COLOR_ICON_NEUROLINK 291
#define APEX_COLOR_ICON_NOW_YOU_SEE_ME 294
#define APEX_COLOR_ICON_NOX_GAS_GRENADE 295
#define APEX_COLOR_ICON_NOX_GAS_TRAP 296
#define APEX_COLOR_ICON_NOX_VISION 297
#define APEX_COLOR_ICON_PANIC_BUTTON 304
#define APEX_COLOR_ICON_PERIMETER_SECURITY 311
#define APEX_COLOR_ICON_PHASE_BREACH 323
#define APEX_COLOR_ICON_PHASE_CHAMBER 324
#define APEX_COLOR_ICON_PIERCING_SPIKES 326
#define APEX_COLOR_ICON_PSYCHE_OUT 355
#define APEX_COLOR_ICON_RADIANT_TRANSFER 361
#define APEX_COLOR_ICON_REDUCED_TACTICAL_RECHARGE 376
#define APEX_COLOR_ICON_REDUCED_ULTIMATE_RECHARGE 377
#define APEX_COLOR_ICON_RETRIEVE_THE_WOUNDED 379
#define APEX_COLOR_ICON_RIOT_DRILL 385
#define APEX_COLOR_ICON_ROLLING_THUNDER 386
#define APEX_COLOR_ICON_ROWDY_S_RAVE 387
#define APEX_COLOR_ICON_SAVIOR_S_SPEED 392
#define APEX_COLOR_ICON_SHADOW_POUNCE 437
#define APEX_COLOR_ICON_SILENCE 446
#define APEX_COLOR_ICON_SKYWARD_DIVE 454
#define APEX_COLOR_ICON_SLING 455
#define APEX_COLOR_ICON_SLIPSTREAM 456
#define APEX_COLOR_ICON_SMOKE_LAUNCHER 457
#define APEX_COLOR_ICON_SNIPER_S_MARK 461
#define APEX_COLOR_ICON_SPACEWALK 462
#define APEX_COLOR_ICON_SPARK_OF_GENIUS 463
#define APEX_COLOR_ICON_SPOTTER_S_LENS 468
#define APEX_COLOR_ICON_STALKER 469
#define APEX_COLOR_ICON_STIM 476
#define APEX_COLOR_ICON_SURVEILLANCE_DRONE 481
#define APEX_COLOR_ICON_SWIFT_MEND 482
#define APEX_COLOR_ICON_TEMPEST 487
#define APEX_COLOR_ICON_THE_MOTHERLODE 488
#define APEX_COLOR_ICON_TRACKER 524
#define APEX_COLOR_ICON_VOICES_FROM_THE_VOID 545
#define APEX_COLOR_ICON_VTOL_JETS 548
#define APEX_COLOR_ICON_WARLORD_S_IRE 551
#define APEX_COLOR_ICON_WHISTLER 556
#define APEX_COLOR_ICON_WRECKING_BALL 566
#define APEX_COLOR_ICON_ZIPLINE_GUN 570

// Ammo Icons
#define APEX_COLOR_ICON_ARROWS 29
#define APEX_COLOR_ICON_ENERGY_AMMO 130
#define APEX_COLOR_ICON_HEAVY_ROUNDS_REVVED_UP 187
#define APEX_COLOR_ICON_HEAVY_ROUNDS 188
#define APEX_COLOR_ICON_HEAVYLIGHT_ROUNDS 189
#define APEX_COLOR_ICON_LIGHT_ROUNDS 233
#define APEX_COLOR_ICON_MYTHIC_ARROWS 280
#define APEX_COLOR_ICON_MYTHIC_ENERGY_AMMO 281
#define APEX_COLOR_ICON_MYTHIC_HEAVY_ROUNDS 282
#define APEX_COLOR_ICON_MYTHIC_LIGHT_ROUNDS 283
#define APEX_COLOR_ICON_MYTHIC_SHOTGUN_SHELLS 284
#define APEX_COLOR_ICON_MYTHIC_SNIPER_AMMO 285
#define APEX_COLOR_ICON_MYTHIC_THROWING_KNIFE_AMMO 286
#define APEX_COLOR_ICON_SHOTGUN_SHELLS 445
#define APEX_COLOR_ICON_SNIPER_AMMO_AMPED 458
#define APEX_COLOR_ICON_SNIPER_AMMO 459

// Attachment Icons
#define APEX_COLOR_ICON_BARREL_STABILIZER_LVL4 48
#define APEX_COLOR_ICON_BARREL_STABILIZER 49
#define APEX_COLOR_ICON_EXTENDED_CAR_MAG 141
#define APEX_COLOR_ICON_EXTENDED_ENERGY_MAG 142
#define APEX_COLOR_ICON_EXTENDED_HEAVY_MAG 143
#define APEX_COLOR_ICON_EXTENDED_LIGHT_MAG 144
#define APEX_COLOR_ICON_EXTENDED_SNIPER_MAG 145
#define APEX_COLOR_ICON_LASER_SIGHT 223
#define APEX_COLOR_ICON_SHOTGUN_BOLT 444
#define APEX_COLOR_ICON_SNIPER_STOCK 460
#define APEX_COLOR_ICON_STANDARD_STOCK 470

// Battle Pass Icons
#define APEX_COLOR_ICON_SEASON_1_MOBILE 393
#define APEX_COLOR_ICON_SEASON_1 395
#define APEX_COLOR_ICON_SEASON_10 397
#define APEX_COLOR_ICON_SEASON_11 398
#define APEX_COLOR_ICON_SEASON_12 400
#define APEX_COLOR_ICON_SEASON_13 402
#define APEX_COLOR_ICON_SEASON_14 403
#define APEX_COLOR_ICON_SEASON_15 404
#define APEX_COLOR_ICON_SEASON_16 405
#define APEX_COLOR_ICON_SEASON_17 406
#define APEX_COLOR_ICON_SEASON_18 407
#define APEX_COLOR_ICON_SEASON_19 408
#define APEX_COLOR_ICON_SEASON_2_MOBILE 409
#define APEX_COLOR_ICON_SEASON_2 411
#define APEX_COLOR_ICON_SEASON_3_MOBILE 412
#define APEX_COLOR_ICON_SEASON_3 414
#define APEX_COLOR_ICON_SEASON_4_MOBILE 415
#define APEX_COLOR_ICON_SEASON_4 417
#define APEX_COLOR_ICON_SEASON_5 418
#define APEX_COLOR_ICON_SEASON_6 420
#define APEX_COLOR_ICON_SEASON_7 422
#define APEX_COLOR_ICON_SEASON_8 424
#define APEX_COLOR_ICON_SEASON_9 425

// Connection Icons
#define APEX_COLOR_ICON_ICON_CONGESTION 199
#define APEX_COLOR_ICON_ICON_LAG_COMPENSATION_ERROR 200
#define APEX_COLOR_ICON_ICON_LATENCY 201
#define APEX_COLOR_ICON_ICON_PACKET_LOSS 202
#define APEX_COLOR_ICON_ICON_PREDICTION_ERROR 203
#define APEX_COLOR_ICON_ICON_UCMD_DELAY 204

// Cosmetic Icons
#define APEX_COLOR_ICON_APEX_PACK_ICON 22
#define APEX_COLOR_ICON_BADGES_ICON 41
#define APEX_COLOR_ICON_BANNER_FRAME_ICON 46
#define APEX_COLOR_ICON_BANNER_POSE_ICON 47
#define APEX_COLOR_ICON_CHARMS_ICON 80
#define APEX_COLOR_ICON_EMOTE_ICON 127
#define APEX_COLOR_ICON_EVENT_APEX_PACK_ICON 134
#define APEX_COLOR_ICON_HOLOSPRAY_ICON 194
#define APEX_COLOR_ICON_INTRO_QUIP_ICON 212
#define APEX_COLOR_ICON_KILL_QUIP_ICON 215
#define APEX_COLOR_ICON_MUSIC_PACK_ICON 279
#define APEX_COLOR_ICON_QUIP_ICON 358
#define APEX_COLOR_ICON_SKIN_ICON 450
#define APEX_COLOR_ICON_SKYDIVE_EMOTE_ICON 453
#define APEX_COLOR_ICON_STAT_TRACKER_ICON 472
#define APEX_COLOR_ICON_STICKER_ICON 475
#define APEX_COLOR_ICON_TRANSITION_ICON 525
#define APEX_COLOR_ICON_WARRIORS_PACK_ICON 552

// Currency Icons
#define APEX_COLOR_ICON_ACTIVITY 12
#define APEX_COLOR_ICON_APEX_COINS 21
#define APEX_COLOR_ICON_CLUB_COINS 85
#define APEX_COLOR_ICON_CRAFTING_METALS 96
#define APEX_COLOR_ICON_FADE_CHIP 148
#define APEX_COLOR_ICON_FLUX 157
#define APEX_COLOR_ICON_HEIRLOOM_SHARDS 190
#define APEX_COLOR_ICON_LEGEND_FRAGMENTS 226
#define APEX_COLOR_ICON_LEGEND_TOKENS 228
#define APEX_COLOR_ICON_ORIGIN_ICON 302
#define APEX_COLOR_ICON_PS4_TROPHY_BRONZE_ICON 351
#define APEX_COLOR_ICON_PS4_TROPHY_GOLD_ICON 352
#define APEX_COLOR_ICON_PS4_TROPHY_SILVER_ICON 353
#define APEX_COLOR_ICON_SYNDICATE_GOLD 484
#define APEX_COLOR_ICON_XBOX_GAMERSCORE_ICON 567

// Event Icons
#define APEX_COLOR_ICON_ANNIVERSARY_ICON 19
#define APEX_COLOR_ICON_AWAKENING_ICON 36
#define APEX_COLOR_ICON_BEAST_OF_PREY_ICON 53
#define APEX_COLOR_ICON_BEAST_OF_PREY 54
#define APEX_COLOR_ICON_CELESTIAL_SUNRISE 74
#define APEX_COLOR_ICON_CHAOS_THEORY_ICON 78
#define APEX_COLOR_ICON_DARK_DEPTHS_ICON 102
#define APEX_COLOR_ICON_DEATH_DYNASTY_ICON 106
#define APEX_COLOR_ICON_DEATH_DYNASTY 107
#define APEX_COLOR_ICON_DOPPELGANGERS 116
#define APEX_COLOR_ICON_DRESSED_TO_KILL_ICON 120
#define APEX_COLOR_ICON_DRESSED_TO_KILL 121
#define APEX_COLOR_ICON_EVOLUTION_ICON 139
#define APEX_COLOR_ICON_FIGHT_NIGHT_ICON 152
#define APEX_COLOR_ICON_FIGHT_OR_FRIGHT_ICON 154
#define APEX_COLOR_ICON_FINAL_FANTASY_VII 155
#define APEX_COLOR_ICON_GAIDEN 166
#define APEX_COLOR_ICON_GENESIS_ICON 168
#define APEX_COLOR_ICON_HARBINGERS 183
#define APEX_COLOR_ICON_HOLO_DAY_BASH_ICON 193
#define APEX_COLOR_ICON_IMPERIAL_GUARD_ICON 207
#define APEX_COLOR_ICON_IRON_CROWN_ICON 213
#define APEX_COLOR_ICON_LEGENDARY_HUNT_ICON 229
#define APEX_COLOR_ICON_MONSTERS_WITHIN_ICON 276
#define APEX_COLOR_ICON_NEON_NETWORK_ICON 289
#define APEX_COLOR_ICON_NEON_NETWORK 290
#define APEX_COLOR_ICON_RAIDERS_ICON 362
#define APEX_COLOR_ICON_SPELLBOUND_ICON 465
#define APEX_COLOR_ICON_SPELLBOUND 466
#define APEX_COLOR_ICON_SUN_SQUAD_ICON 477
#define APEX_COLOR_ICON_SUN_SQUAD 478
#define APEX_COLOR_ICON_THE_OLD_WAYS_ICON 489
#define APEX_COLOR_ICON_THREAT_LEVEL_ICON 492
#define APEX_COLOR_ICON_THREAT_LEVEL 493
#define APEX_COLOR_ICON_THRILLSEEKERS_ICON 494
#define APEX_COLOR_ICON_UNSHACKLED_ICON 534
#define APEX_COLOR_ICON_UPRISING_ICON 535
#define APEX_COLOR_ICON_UPRISING 536
#define APEX_COLOR_ICON_VALENTINES_EVENT_ICON 537
#define APEX_COLOR_ICON_VEILED_ICON 541
#define APEX_COLOR_ICON_VEILED 542
#define APEX_COLOR_ICON_VOIDWALKER_ICON 546
#define APEX_COLOR_ICON_WAR_GAMES_ICON 550
#define APEX_COLOR_ICON_WARRIORS 553
#define APEX_COLOR_ICON_WINTERTIDE 559

// Gear Icons
#define APEX_COLOR_ICON_BACKPACK_LVL1 37
#define APEX_COLOR_ICON_BACKPACK_LVL2 38
#define APEX_COLOR_ICON_BACKPACK_LVL3 39
#define APEX_COLOR_ICON_BODY_SHIELD 62
#define APEX_COLOR_ICON_EVAC_TOWER 133
#define APEX_COLOR_ICON_EVO_SHIELD_LEVEL_2 135
#define APEX_COLOR_ICON_EVO_SHIELD_LEVEL_3 136
#define APEX_COLOR_ICON_EVO_SHIELD_LEVEL_5 137
#define APEX_COLOR_ICON_EVO_SHIELD 138
#define APEX_COLOR_ICON_HEAT_SHIELD 186
#define APEX_COLOR_ICON_HELMET 191
#define APEX_COLOR_ICON_KNOCKDOWN_SHIELD 218
#define APEX_COLOR_ICON_MOBILE_RESPAWN_BEACON 272

// Hop Up Icons
#define APEX_COLOR_ICON_ANVIL_RECEIVER 20
#define APEX_COLOR_ICON_BOOSTED_LOADER 63
#define APEX_COLOR_ICON_DEADEYE_S_TEMPO 105
#define APEX_COLOR_ICON_DISRUPTOR_ROUNDS 114
#define APEX_COLOR_ICON_DOUBLE_TAP_TRIGGER 117
#define APEX_COLOR_ICON_DUAL_SHELL 123
#define APEX_COLOR_ICON_HAMMERPOINT_ROUNDS 181
#define APEX_COLOR_ICON_HOP_UP_SLOT 195
#define APEX_COLOR_ICON_KINETIC_FEEDER 217
#define APEX_COLOR_ICON_PRECISION_CHOKE 347
#define APEX_COLOR_ICON_QUICKDRAW_HOLSTER 357
#define APEX_COLOR_ICON_SELECTFIRE_RECEIVER 435
#define APEX_COLOR_ICON_SHATTER_CAPS 440
#define APEX_COLOR_ICON_SKULLPIERCER_RIFLING 452
#define APEX_COLOR_ICON_SPLATTER_ROUNDS 467
#define APEX_COLOR_ICON_TURBOCHARGER 530

// Inventory Icons
#define APEX_COLOR_ICON_GOLDEN_TICKET 173
#define APEX_COLOR_ICON_MRVN_ARM 278
#define APEX_COLOR_ICON_OPTICS_SLOT 301
#define APEX_COLOR_ICON_TREASURE_PACK_ICON 526
#define APEX_COLOR_ICON_VAULT_KEY 540

// Legend Class Icons
#define APEX_COLOR_ICON_ASSAULT_CLASS 32
#define APEX_COLOR_ICON_CONTROLLER_CLASS 92
#define APEX_COLOR_ICON_DEFENSIVE 110
#define APEX_COLOR_ICON_OFFENSIVE 300
#define APEX_COLOR_ICON_RECON_CLASS 374
#define APEX_COLOR_ICON_RECON_OLD 375
#define APEX_COLOR_ICON_SKIRMISHER_CLASS 451
#define APEX_COLOR_ICON_SUPPORT_CLASS 479
#define APEX_COLOR_ICON_SUPPORT_OLD 480

// Legend Icons
#define APEX_COLOR_ICON_ASH_ICON 30
#define APEX_COLOR_ICON_BALLISTIC_ICON 42
#define APEX_COLOR_ICON_BANGALORE_ICON 43
#define APEX_COLOR_ICON_BLOODHOUND_ICON 58
#define APEX_COLOR_ICON_CATALYST_ICON 71
#define APEX_COLOR_ICON_CAUSTIC_ICON 72
#define APEX_COLOR_ICON_CONDUIT_ICON 89
#define APEX_COLOR_ICON_CRYPTO_ICON 97
#define APEX_COLOR_ICON_FADE_ICON 149
#define APEX_COLOR_ICON_FUSE_ICON 164
#define APEX_COLOR_ICON_GIBRALTAR_ICON 169
#define APEX_COLOR_ICON_GIBRALTAR_MOBILE_ICON 170
#define APEX_COLOR_ICON_HORIZON_ICON 196
#define APEX_COLOR_ICON_LIFELINE_ICON 231
#define APEX_COLOR_ICON_LOBA_ICON 234
#define APEX_COLOR_ICON_MAD_MAGGIE_ICON 243
#define APEX_COLOR_ICON_MIRAGE_ICON 267
#define APEX_COLOR_ICON_MIRAGE_MOBILE_ICON 268
#define APEX_COLOR_ICON_NEWCASTLE_ICON 292
#define APEX_COLOR_ICON_OCTANE_ICON 298
#define APEX_COLOR_ICON_PATHFINDER_ICON 307
#define APEX_COLOR_ICON_PATHFINDER_MOBILE_ICON 308
#define APEX_COLOR_ICON_RAMPART_ICON 364
#define APEX_COLOR_ICON_REVENANT_ICON 381
#define APEX_COLOR_ICON_RHAPSODY_ICON 383
#define APEX_COLOR_ICON_SEER_ICON 434
#define APEX_COLOR_ICON_VALKYRIE_ICON 538
#define APEX_COLOR_ICON_VANTAGE_ICON 539
#define APEX_COLOR_ICON_WATTSON_ICON 554
#define APEX_COLOR_ICON_WRAITH_ICON 562
#define APEX_COLOR_ICON_WRAITH_MOBILE_ICON 563

// Loadout Icons
#define APEX_COLOR_ICON_ALGS_ICON 15
#define APEX_COLOR_ICON_ARENAS_ICON 26
#define APEX_COLOR_ICON_BATTLE_ROYALE 52
#define APEX_COLOR_ICON_CAMO_CREDITS_ICON 68
#define APEX_COLOR_ICON_CHAMPION_EDITION_ICON 76
#define APEX_COLOR_ICON_COMMUNITY_CONTENT_ICON 88
#define APEX_COLOR_ICON_EA_ICON 124
#define APEX_COLOR_ICON_FOUNDERS_PACK_ICON 161
#define APEX_COLOR_ICON_NINTENDO_SWITCH_ICON 293
#define APEX_COLOR_ICON_PRIME_GAMING 348
#define APEX_COLOR_ICON_PSPLUS_LOGO 354
#define APEX_COLOR_ICON_SALE_ICON 388
#define APEX_COLOR_ICON_SEASON_1_ICON 394
#define APEX_COLOR_ICON_SEASON_10_ICON 396
#define APEX_COLOR_ICON_SEASON_13_ICON 401
#define APEX_COLOR_ICON_SEASON_2_ICON 410
#define APEX_COLOR_ICON_SEASON_3_ICON 413
#define APEX_COLOR_ICON_SEASON_4_ICON 416
#define APEX_COLOR_ICON_SEASON_6_ICON 419
#define APEX_COLOR_ICON_SEASON_7_ICON 421
#define APEX_COLOR_ICON_SEASON_8_ICON 423
#define APEX_COLOR_ICON_STARTER_PACK_ICON 471
#define APEX_COLOR_ICON_STEAM_ICON 473
#define APEX_COLOR_ICON_STEAM_LOGO 474
#define APEX_COLOR_ICON_TWITCH_LOGO 531

// Lore Icons
#define APEX_COLOR_ICON_APEX_PREDATORS 23
#define APEX_COLOR_ICON_BURRELL_DEFENSE 65
#define APEX_COLOR_ICON_CHEVREX 82
#define APEX_COLOR_ICON_CLEO_RECOVERY_COUNCIL 84
#define APEX_COLOR_ICON_CRACKED_TALON 93
#define APEX_COLOR_ICON_CRYPTO_SYMBOL_ICON 98
#define APEX_COLOR_ICON_DRAGON_COMPANY 119
#define APEX_COLOR_ICON_ECHO_ICON 125
#define APEX_COLOR_ICON_EMSLIE_TACTICAL_ICON 129
#define APEX_COLOR_ICON_FRONTIER_CORPS_ICON 163
#define APEX_COLOR_ICON_HAMMOND_ROBOTICS_ICON 182
#define APEX_COLOR_ICON_IMC_ICON_1 205
#define APEX_COLOR_ICON_IMC_STORM_POINT_ICON 206
#define APEX_COLOR_ICON_INTERSTELLAR_MANUFACTURING_CORPORATION 210
#define APEX_COLOR_ICON_KODAI 220
#define APEX_COLOR_ICON_LASTIMOSA_ARMORY 224
#define APEX_COLOR_ICON_MERCENARY_SYNDICATE 266
#define APEX_COLOR_ICON_NATIONAL_ALLIANCE_OF_BOREAS 287
#define APEX_COLOR_ICON_PARADINHA_ARSENAL_ICON 305
#define APEX_COLOR_ICON_PROTEUS_ICON 349
#define APEX_COLOR_ICON_RAMPART_SYMBOL_ICON 365
#define APEX_COLOR_ICON_SALVO_ICON 389
#define APEX_COLOR_ICON_SALVO_MC_ICON 390
#define APEX_COLOR_ICON_SARAS_ICON 391
#define APEX_COLOR_ICON_SEASON_12_IMC 399
#define APEX_COLOR_ICON_SILVA_PHARMACEUTICALS_ICON 447
#define APEX_COLOR_ICON_SIWHAN_INDUSTRIES 449
#define APEX_COLOR_ICON_SYNDICATE_CORPS_ICON 483
#define APEX_COLOR_ICON_THE_SISTERS 490
#define APEX_COLOR_ICON_VINSON_DYNAMICS_ICON 543
#define APEX_COLOR_ICON_WONYEON 560

// Map Icons
#define APEX_COLOR_ICON_MAPICON_CARE_PACKAGE 244
#define APEX_COLOR_ICON_MAPICON_DROPSHIP 245
#define APEX_COLOR_ICON_MAPICON_INTERCEPTION_PYLON 246
#define APEX_COLOR_ICON_MAPICON_NESSY 247
#define APEX_COLOR_ICON_MAPICON_PLAYER 248
#define APEX_COLOR_ICON_MAPICON_PROWLER_NEST 249
#define APEX_COLOR_ICON_MAPICON_RESPAWN_BEACON 250
#define APEX_COLOR_ICON_MAPICON_RING_CONSOLE 251
#define APEX_COLOR_ICON_MAPICON_SHADOWFALL_KILLER 252
#define APEX_COLOR_ICON_MAPICON_SHADOWFALL_LEGEND_DEATH 253
#define APEX_COLOR_ICON_MAPICON_SHADOWFALL_SHADOW_DEATH 254
#define APEX_COLOR_ICON_MAPICON_SPIDER_NEST 255
#define APEX_COLOR_ICON_MAPICON_SUPPLY_SHIP_LANDING_SPOT 256
#define APEX_COLOR_ICON_MAPICON_SUPPLY_SHIP 257
#define APEX_COLOR_ICON_MAPICON_SURVEY_BEACON 258
#define APEX_COLOR_ICON_MAPICON_TRAIN 259
#define APEX_COLOR_ICON_MAPICON_TRIDENT_SPAWN 260
#define APEX_COLOR_ICON_MAPICON_VAULT 261

// Mobile Stat Tracker Icons
#define APEX_COLOR_ICON_BANGALORE_MOBILE_STAT_TRACKER_ICON 44
#define APEX_COLOR_ICON_BLOODHOUND_MOBILE_STAT_TRACKER_ICON 59
#define APEX_COLOR_ICON_CAUSTIC_MOBILE_STAT_TRACKER_ICON 73
#define APEX_COLOR_ICON_FADE_MOBILE_STAT_TRACKER_ICON 150
#define APEX_COLOR_ICON_GIBRALTAR_MOBILE_STAT_TRACKER_ICON 171
#define APEX_COLOR_ICON_LIFELINE_MOBILE_STAT_TRACKER_ICON 232
#define APEX_COLOR_ICON_LOBA_MOBILE_STAT_TRACKER_ICON 235
#define APEX_COLOR_ICON_MIRAGE_MOBILE_STAT_TRACKER_ICON 269
#define APEX_COLOR_ICON_OCTANE_MOBILE_STAT_TRACKER_ICON 299
#define APEX_COLOR_ICON_PATHFINDER_MOBILE_STAT_TRACKER_ICON 309
#define APEX_COLOR_ICON_WRAITH_MOBILE_STAT_TRACKER_ICON 564

// Optics Icons
#define APEX_COLOR_ICON_1X_DIGITAL_THREAT 0
#define APEX_COLOR_ICON_1X_HCOG_CLASSIC 1
#define APEX_COLOR_ICON_1X_HOLO 2
#define APEX_COLOR_ICON_1X_2X_VARIABLE_HOLO 3
#define APEX_COLOR_ICON_2X_HCOG_BRUISER 4
#define APEX_COLOR_ICON_2X_4X_VARIABLE_AOG 5
#define APEX_COLOR_ICON_3X_HCOG_RANGER 7
#define APEX_COLOR_ICON_4X_10X_DIGITAL_SNIPER_THREAT 8
#define APEX_COLOR_ICON_4X_8X_VARIABLE_SNIPER 9
#define APEX_COLOR_ICON_6X_SNIPER 10

// Perk Coin Icons
#define APEX_COLOR_ICON_PERK_COIN_BANGALORE 312
#define APEX_COLOR_ICON_PERK_COIN_BLOODHOUND 313
#define APEX_COLOR_ICON_PERK_COIN_CAUSTIC 314
#define APEX_COLOR_ICON_PERK_COIN_FADE 315
#define APEX_COLOR_ICON_PERK_COIN_GIBRALTAR 316
#define APEX_COLOR_ICON_PERK_COIN_LIFELINE 317
#define APEX_COLOR_ICON_PERK_COIN_LOBA 318
#define APEX_COLOR_ICON_PERK_COIN_MIRAGE 319
#define APEX_COLOR_ICON_PERK_COIN_OCTANE 320
#define APEX_COLOR_ICON_PERK_COIN_PATHFINDER 321
#define APEX_COLOR_ICON_PERK_COIN_WRAITH 322

// Ping Icons
#define APEX_COLOR_ICON_PING_ATTACKING_HERE 327
#define APEX_COLOR_ICON_PING_BASIC 328
#define APEX_COLOR_ICON_PING_DEATHBOX_BANNER 329
#define APEX_COLOR_ICON_PING_DECOY_SHOT 330
#define APEX_COLOR_ICON_PING_DEFENDING_HERE 331
#define APEX_COLOR_ICON_PING_DIBS 332
#define APEX_COLOR_ICON_PING_ENEMY_HERE 333
#define APEX_COLOR_ICON_PING_FENCE_CROSSED 334
#define APEX_COLOR_ICON_PING_GOING_HERE 335
#define APEX_COLOR_ICON_PING_HELP 336
#define APEX_COLOR_ICON_PING_I_CAN_T 337
#define APEX_COLOR_ICON_PING_JOIN 338
#define APEX_COLOR_ICON_PING_LOOTING_HERE 339
#define APEX_COLOR_ICON_PING_NICE 340
#define APEX_COLOR_ICON_PING_NO 341
#define APEX_COLOR_ICON_PING_OK 342
#define APEX_COLOR_ICON_PING_RECENT_BATTLE 343
#define APEX_COLOR_ICON_PING_SOMEONES_BEEN_HERE 344
#define APEX_COLOR_ICON_PING_WATCHING_HERE 345

// Platform Icons
#define APEX_COLOR_ICON_PLAYSTATION_ICON 346
#define APEX_COLOR_ICON_XBOX_ICON 568

// Ranked Tier Icons
#define APEX_COLOR_ICON_RANKED_TIER1_BRONZE 366
#define APEX_COLOR_ICON_RANKED_TIER2_SILVER 367
#define APEX_COLOR_ICON_RANKED_TIER3_GOLD 368
#define APEX_COLOR_ICON_RANKED_TIER4_PLATINUM 369
#define APEX_COLOR_ICON_RANKED_TIER5_DIAMOND 370
#define APEX_COLOR_ICON_RANKED_TIER6_MASTER 371
#define APEX_COLOR_ICON_RANKED_TIER7_APEX_PREDATOR 372

// Regen Icons
#define APEX_COLOR_ICON_MED_KIT 265
#define APEX_COLOR_ICON_PHOENIX_KIT 325
#define APEX_COLOR_ICON_SHIELD_BATTERY 442
#define APEX_COLOR_ICON_SHIELD_CELL 443
#define APEX_COLOR_ICON_SYRINGE 485
#define APEX_COLOR_ICON_ULTIMATE_ACCELERANT 532

// Seasonal Currency Icons
#define APEX_COLOR_ICON_SEASONAL_CURRENCY_S1_5 426
#define APEX_COLOR_ICON_SEASONAL_CURRENCY_S1 427
#define APEX_COLOR_ICON_SEASONAL_CURRENCY_S2_5 428
#define APEX_COLOR_ICON_SEASONAL_CURRENCY_S2 429
#define APEX_COLOR_ICON_SEASONAL_CURRENCY_S3_5 430
#define APEX_COLOR_ICON_SEASONAL_CURRENCY_S3 431
#define APEX_COLOR_ICON_SEASONAL_CURRENCY_S4 432

// Thematic Pack Icons
#define APEX_COLOR_ICON_BANGALORE_THEMATIC_PACK_ICON 45
#define APEX_COLOR_ICON_BLOODHOUND_THEMATIC_PACK_ICON 60
#define APEX_COLOR_ICON_CHARMS_THEMATIC_PACK_ICON 81
#define APEX_COLOR_ICON_CRYPTO_THEMATIC_PACK_ICON 99
#define APEX_COLOR_ICON_EMOTE_THEMATIC_PACK_ICON 128
#define APEX_COLOR_ICON_REVENANT_THEMATIC_PACK_ICON 382
#define APEX_COLOR_ICON_WRAITH_THEMATIC_PACK_ICON 565

// Throwables Icons
#define APEX_COLOR_ICON_ARC_STAR 25
#define APEX_COLOR_ICON_FRAG_GRENADE 162
#define APEX_COLOR_ICON_THERMITE_GRENADE 491

// Tracker Icons
#define APEX_COLOR_ICON_TRACKER_ICON_ABILITY_PLACED 496
#define APEX_COLOR_ICON_TRACKER_ICON_BLOOD 497
#define APEX_COLOR_ICON_TRACKER_ICON_DOOR_DESTROYED 498
#define APEX_COLOR_ICON_TRACKER_ICON_DOOR_USED 499
#define APEX_COLOR_ICON_TRACKER_ICON_EMPTY_SHELLS 500
#define APEX_COLOR_ICON_TRACKER_ICON_ENTERED_PHASE 501
#define APEX_COLOR_ICON_TRACKER_ICON_FOOTPRINT 502
#define APEX_COLOR_ICON_TRACKER_ICON_GAS_RESIDUE 503
#define APEX_COLOR_ICON_TRACKER_ICON_GRENADE_PIN 504
#define APEX_COLOR_ICON_TRACKER_ICON_INTERCEPTION_PYLON_DESTROYED 505
#define APEX_COLOR_ICON_TRACKER_ICON_ITEM_DISCARDED 506
#define APEX_COLOR_ICON_TRACKER_ICON_ITEM_SWAPPED 507
#define APEX_COLOR_ICON_TRACKER_ICON_KILLED_BY_WEAPON 508
#define APEX_COLOR_ICON_TRACKER_ICON_KILLER_WITH_WEAPON 509
#define APEX_COLOR_ICON_TRACKER_ICON_LANDED 510
#define APEX_COLOR_ICON_TRACKER_ICON_LEFT_PHASE 511
#define APEX_COLOR_ICON_TRACKER_ICON_LEFT_ZIPLINE 512
#define APEX_COLOR_ICON_TRACKER_ICON_LOOT_BIN_OPENED 513
#define APEX_COLOR_ICON_TRACKER_ICON_MANTLED 514
#define APEX_COLOR_ICON_TRACKER_ICON_PERIMETER_SECURITY_DESTROYED 515
#define APEX_COLOR_ICON_TRACKER_ICON_PICKED_UP_ITEM 516
#define APEX_COLOR_ICON_TRACKER_ICON_POD_OPENED 517
#define APEX_COLOR_ICON_TRACKER_ICON_SEVERE_BLEEDING 518
#define APEX_COLOR_ICON_TRACKER_ICON_SLIDE_MARK 519
#define APEX_COLOR_ICON_TRACKER_ICON_SMOKE_RESIDUE 520
#define APEX_COLOR_ICON_TRACKER_ICON_WEAPON_RELOADED 521
#define APEX_COLOR_ICON_TRACKER_ICON_WOUND_BANDAGED 522
#define APEX_COLOR_ICON_TRACKER_ICON_ZIPLINED 523

// UI Icons
#define APEX_COLOR_ICON_AFTERMARKET_ICON 13
#define APEX_COLOR_ICON_AIRDROP_ESCALATION_ICON 14
#define APEX_COLOR_ICON_ANNIVERSARY_EVENT_ICON 18
#define APEX_COLOR_ICON_ARMED_AND_DANGEROUS_ICON 27
#define APEX_COLOR_ICON_ARMOR_REGEN_ICON 28
#define APEX_COLOR_ICON_ASSISTS_ICON 33
#define APEX_COLOR_ICON_AUTO_BANNERS_ICON 34
#define APEX_COLOR_ICON_AUTO_ICON 35
#define APEX_COLOR_ICON_BADGE_FIGHT_OR_FRIGHT 40
#define APEX_COLOR_ICON_BATTLE_PASS_LEVEL 51
#define APEX_COLOR_ICON_BURST_ICON 66
#define APEX_COLOR_ICON_CHALLENGE_STARS 75
#define APEX_COLOR_ICON_CHAOS_THEORY_EVENT_ICON 77
#define APEX_COLOR_ICON_CHOKE_ACTIVE_ICON 83
#define APEX_COLOR_ICON_CLUB_ICON 86
#define APEX_COLOR_ICON_CONTROL_ICON 90
#define APEX_COLOR_ICON_CONTROL 91
#define APEX_COLOR_ICON_CRAFTING_EVO 94
#define APEX_COLOR_ICON_CRAFTING_ICON 95
#define APEX_COLOR_ICON_DAMAGE 101
#define APEX_COLOR_ICON_DEADEYE_S_TEMPO_ACTIVE_ICON 104
#define APEX_COLOR_ICON_DEJA_LOOT_ICON 111
#define APEX_COLOR_ICON_FIGHT_NIGHT_EVENT_ICON 151
#define APEX_COLOR_ICON_FIGHT_OR_FRIGHT_2020_ICON 153
#define APEX_COLOR_ICON_GENESIS_EVENT_ICON 167
#define APEX_COLOR_ICON_GRAND_SOIREE_ICON 174
#define APEX_COLOR_ICON_GUN_RUN_ICON 178
#define APEX_COLOR_ICON_GUN_RUN 179
#define APEX_COLOR_ICON_ICON_CHAMPION 198
#define APEX_COLOR_ICON_KILL_LEADER_ICON 214
#define APEX_COLOR_ICON_KILLS_ICON 216
#define APEX_COLOR_ICON_LEGEND_MASTERY_ICON 227
#define APEX_COLOR_ICON_LOCKED_AND_LOADED_ICON 236
#define APEX_COLOR_ICON_LOST_TREASURES_ICON 239
#define APEX_COLOR_ICON_LTM_ICON 241
#define APEX_COLOR_ICON_MATERIALS_BLUE 264
#define APEX_COLOR_ICON_MOBILE_TEAM_DEATHMATCH 274
#define APEX_COLOR_ICON_PARTICIPATION_ICON 306
#define APEX_COLOR_ICON_QUEST_ICON 356
#define APEX_COLOR_ICON_REPLICATOR_ICON 378
#define APEX_COLOR_ICON_REVENANT_ARMY_ICON 380
#define APEX_COLOR_ICON_RING_FURY_ICON 384
#define APEX_COLOR_ICON_SECOND_CHANCE_ICON 433
#define APEX_COLOR_ICON_SHADOWFALL_ICON 438
#define APEX_COLOR_ICON_SHATTER_CAPS_ACTIVE_ICON 439
#define APEX_COLOR_ICON_SINGLE_FIRE_ICON 448
#define APEX_COLOR_ICON_SPECTATOR_ICON 464
#define APEX_COLOR_ICON_SYSTEM_OVERRIDE_ICON 486
#define APEX_COLOR_ICON_TRIALS_ICON 527
#define APEX_COLOR_ICON_TRIDENT_SPAWN_LOCATION_ICON 528
#define APEX_COLOR_ICON_ULTRA_ZONES_ICON 533
#define APEX_COLOR_ICON_WAR_GAMES_EVENT_ICON 549
#define APEX_COLOR_ICON_WEAPON_EXPERIENCE_ICON 555
#define APEX_COLOR_ICON_WINTER_EXPRESS_ICON 558
#define APEX_COLOR_ICON_WORLD_S_EDGE_MOBILE_ICON 561
#define APEX_COLOR_ICON_XP_BOOST_ICON 569

// Weapon Icons
#define APEX_COLOR_ICON_30_30_REPEATER_ICON 6
#define APEX_COLOR_ICON_A_13_SENTRY_ICON 11
#define APEX_COLOR_ICON_ALTERNATOR_SMG_ICON 16
#define APEX_COLOR_ICON_BOCEK_COMPOUND_BOW_ICON 61
#define APEX_COLOR_ICON_C_A_R_SMG_ICON 67
#define APEX_COLOR_ICON_CHARGE_RIFLE_ICON 79
#define APEX_COLOR_ICON_DEVOTION_LMG_ICON 112
#define APEX_COLOR_ICON_EVA_8_AUTO_ICON 132
#define APEX_COLOR_ICON_G7_SCOUT_ICON 165
#define APEX_COLOR_ICON_HAVOC_RIFLE_ICON 184
#define APEX_COLOR_ICON_HEMLOK_BURST_AR_ICON 192
#define APEX_COLOR_ICON_KRABER_50_CAL_SNIPER_ICON 221
#define APEX_COLOR_ICON_L_STAR_EMG_ICON 222
#define APEX_COLOR_ICON_LONGBOW_DMR_ICON 237
#define APEX_COLOR_ICON_M600_SPITFIRE_ICON 242
#define APEX_COLOR_ICON_MASTIFF_SHOTGUN_ICON 263
#define APEX_COLOR_ICON_MOZAMBIQUE_SHOTGUN_ICON 277
#define APEX_COLOR_ICON_NEMESIS_BURST_AR_ICON 288
#define APEX_COLOR_ICON_P2020_ICON 303
#define APEX_COLOR_ICON_PEACEKEEPER_ICON 310
#define APEX_COLOR_ICON_PROWLER_BURST_PDW_ICON 350
#define APEX_COLOR_ICON_R_301_CARBINE_ICON 359
#define APEX_COLOR_ICON_R_99_SMG_ICON 360
#define APEX_COLOR_ICON_RAMPAGE_LMG_ICON 363
#define APEX_COLOR_ICON_RE_45_AUTO_ICON 373
#define APEX_COLOR_ICON_SENTINEL_ICON 436
#define APEX_COLOR_ICON_SHEILA_ICON 441
#define APEX_COLOR_ICON_THROWING_KNIFE_ICON 495
#define APEX_COLOR_ICON_TRIPLE_TAKE_ICON 529
#define APEX_COLOR_ICON_VK_47_FLATLINE_ICON 544
#define APEX_COLOR_ICON_VOLT_SMG_ICON 547
#define APEX_COLOR_ICON_WINGMAN_ICON 557

inline const ApexColorAtlasIcon& GetApexColorAtlasIcon(int index) {
    return kApexColorAtlasIcons[index];
}

inline void DrawApexColorAtlasIcon(const ImTextureID* page_textures, int index, ImVec2 size = ImVec2(0, 0), ImU32 tint = IM_COL32_WHITE, ImU32 border = 0) {
    const ApexColorAtlasIcon& icon = kApexColorAtlasIcons[index];
    const ImVec2 draw_size = (size.x > 0.0f && size.y > 0.0f) ? size : icon.size;
    ImGui::Image(page_textures[icon.page], draw_size, icon.uv0, icon.uv1, ImGui::ColorConvertU32ToFloat4(tint), ImGui::ColorConvertU32ToFloat4(border));
}
