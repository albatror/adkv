use imgui::*;

pub struct Overlay {
    pub show_menu: bool,
}

impl Overlay {
    pub fn new() -> Self {
        Self { show_menu: false }
    }

    pub fn render(&mut self, ui: &Ui) {
        if self.show_menu {
            ui.window("Apex Rust Rewrite")
                .size([400.0, 300.0], Condition::FirstUseEver)
                .build(|| {
                    ui.text("Visuals");
                    ui.separator();
                    // Add checkboxes for ESP, Glow, etc.
                });
        }
    }
}
