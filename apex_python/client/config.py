# Config settings
class Config:
    def __init__(self):
        self.aim = 0
        self.esp = False
        self.item_glow = False
        self.player_glow = False
        self.aim_no_recoil = True
        self.lock_target = False
        self.max_dist = 120.0 * 40.0
        self.aim_dist = 120.0 * 40.0
        self.default_smooth = 200.0
        self.default_fov = 3.8
        self.bone = 2
        self.healthbar = False
        self.shieldbar = False
        self.distance = False
        self.line = False
        self.skeleton = False
        self.spectator_notifier = False
        self.info_window = False
        self.target_indicator = False
        self.target_indicator_fov = 10.0
        self.glowr = 100.0
        self.glowg = 0.0
        self.glowb = 0.0
        self.glowcolor = [0.0, 0.0, 0.0]
        self.glowrviz = 0.0
        self.glowgviz = 100.0
        self.glowbviz = 0.0
        self.glowcolorviz = [0.0, 0.0, 0.0]
        self.glowrknocked = 100.0
        self.glowgknocked = 100.0
        self.glowbknocked = 100.0
        self.glowcolorknocked = [0.0, 0.0, 0.0]
        self.insidevalue = 6
        self.outlinesize = 32
        self.firing_range = False
        self.onevone = False
        self.DDS = 70.0 * 40.0
        self.min_max_fov = 4.0
        self.max_max_fov = 25.0
        self.min_smooth = 100.0
        self.max_smooth = 150.0
        self.min_cfsize = 4.0
        self.max_cfsize = 25.0
        self.flickbot = False
        self.triggerbot = False
        self.superglide = False
        self.bhop = False
        self.walljump = False
        self.flickbot_fov = 10.0
        self.flickbot_smooth = 20.0
        self.flickbot_auto_shoot = False
        self.flickbot_auto_shoot_delay = 50
        self.flickbot_flickback = False
        self.flickbot_flickback_delay = 10
        self.flickbot_delay = 500
        self.triggerbot_fov = 10.0
        self.flickbot_fov_circle = False
        self.triggerbot_fov_circle = False
        self.fov = False
        self.cfsize = 3.8

    def save(self, filename):
        with open(filename, 'w') as f:
            for k, v in self.__dict__.items():
                if isinstance(v, bool):
                    f.write(f"{k} {str(v).lower()}\n")
                elif isinstance(v, list):
                    f.write(f"{k} {' '.join(map(str, v))}\n")
                else:
                    f.write(f"{k} {v}\n")

    def load(self, filename):
        try:
            with open(filename, 'r') as f:
                for line in f:
                    parts = line.split()
                    if not parts: continue
                    key = parts[0]
                    if key in self.__dict__:
                        if isinstance(self.__dict__[key], bool):
                            self.__dict__[key] = parts[1].lower() == 'true'
                        elif isinstance(self.__dict__[key], int):
                            self.__dict__[key] = int(parts[1])
                        elif isinstance(self.__dict__[key], float):
                            self.__dict__[key] = float(parts[1])
                        elif isinstance(self.__dict__[key], list):
                            self.__dict__[key] = [float(x) for x in parts[1:]]
        except FileNotFoundError:
            pass
