#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>
#include <algorithm>
#include <array>
#include <string>

struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float px, float py) : x(px), y(py) {}

    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    Vec2 operator*(float s) const { return Vec2(x * s, y * s); }
    float dot(const Vec2& v) const { return x * v.x + y * v.y; }
    float length() const { return std::sqrt(x * x + y * y); }
    Vec2 normalized() const {
        float len = length();
        if (len < 0.0001f) return Vec2(0, 1);
        return Vec2(x / len, y / len);
    }
};

struct Particle {
    Vec2 position;
    Vec2 velocity;
    float radius;
    float gravity;
    float damping;
    bool is_resting;
    Vec2 initial_position;
};

struct Boundary {
    std::vector<Vec2> points;
    Vec2 barycenter;
};

struct CollisionData {
    bool is_colliding;
    float normal_x;
    float normal_y;
    float penetration;
    int other_particle_idx;
};

struct SimulationState {
    std::vector<Particle> particles;
    std::vector<Boundary> boundaries;
    bool is_playing;
    float delta_time;
};

Vec2 calculate_barycenter(const std::vector<Vec2>& points) {
    Vec2 center(0, 0);
    for (const auto& p : points) {
        center.x += p.x;
        center.y += p.y;
    }
    if (!points.empty()) {
        center.x /= points.size();
        center.y /= points.size();
    }
    return center;
}

void update_particle(Particle& p, float dt) {
    if (dt <= 0) return;

    p.velocity.y += p.gravity * dt * 100;
    p.position.x += p.velocity.x * dt;
    p.position.y += p.velocity.y * dt;
}

CollisionData detect_sphere_sphere_collision(
    const Vec2& pos1, float radius1,
    const Vec2& pos2, float radius2,
    int other_idx)
{
    CollisionData data;
    data.other_particle_idx = other_idx;
    data.is_colliding = false;
    data.normal_x = 0;
    data.normal_y = 0;
    data.penetration = 0;

    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    float dist2 = dx * dx + dy * dy;
    float r = radius1 + radius2;

    if (dist2 < r * r) {
        data.is_colliding = true;

        if (dist2 > 0.0001f) {
            float dist = std::sqrt(dist2);
            data.penetration = r - dist;
            data.normal_x = dx / dist;
            data.normal_y = dy / dist;
        } else {
            data.penetration = r;
            data.normal_x = 0.0f;
            data.normal_y = 1.0f;
        }
    }

    return data;
}

bool point_in_polygon(const Vec2& point, const std::vector<Vec2>& polygon) {
    int n = polygon.size();
    int crossings = 0;

    for (int i = 0; i < n; i++) {
        const Vec2& p1 = polygon[i];
        const Vec2& p2 = polygon[(i + 1) % n];

        if ((p1.y <= point.y && point.y < p2.y) ||
            (p2.y <= point.y && point.y < p1.y)) {
            float xinters = (p2.x - p1.x) * (point.y - p1.y) / (p2.y - p1.y) + p1.x;
            if (point.x < xinters) crossings++;
        }
    }

    return crossings % 2 == 1;
}

CollisionData detect_sphere_polyline_collision(
    const Vec2& sphere_pos, float radius,
    const std::vector<Vec2>& polygon)
{
    CollisionData data;
    data.is_colliding = false;
    data.normal_x = 0;
    data.normal_y = 0;
    data.penetration = 0;
    data.other_particle_idx = -1;

    float min_dist = radius;
    Vec2 closest_normal(0, 1);

    int n = polygon.size();
    for (int i = 0; i < n; i++) {
        const Vec2& p1 = polygon[i];
        const Vec2& p2 = polygon[(i + 1) % n];

        Vec2 edge = p2 - p1;
        Vec2 to_sphere = sphere_pos - p1;
        float edge_len2 = edge.dot(edge);

        float t = 0;
        if (edge_len2 > 0.0001f) {
            t = to_sphere.dot(edge) / edge_len2;
            t = std::max(0.0f, std::min(1.0f, t));
        }

        Vec2 closest = p1 + edge * t;
        Vec2 to_closest = sphere_pos - closest;
        float dist = to_closest.length();

        if (dist < min_dist) {
            min_dist = dist;
            closest_normal = to_closest.normalized();
            data.is_colliding = true;
            data.penetration = radius - dist;
            data.normal_x = closest_normal.x;
            data.normal_y = closest_normal.y;
        }
    }

    return data;
}

void resolve_particle_boundary_collision(
    Particle& p,
    const CollisionData& collision)
{
    if (!collision.is_colliding) return;

    p.position.x += collision.normal_x * collision.penetration;
    p.position.y += collision.normal_y * collision.penetration;

    float dot_product = p.velocity.x * collision.normal_x +
                       p.velocity.y * collision.normal_y;

    if (dot_product < 0.0f) {
        p.velocity.x -= 2.0f * dot_product * collision.normal_x;
        p.velocity.y -= 2.0f * dot_product * collision.normal_y;
        p.velocity.x *= p.damping;
        p.velocity.y *= p.damping;
    }
}

void resolve_particle_particle_collision(
    Particle& p1, Particle& p2,
    const CollisionData& collision)
{
    if (!collision.is_colliding) return;

    p1.position.x += collision.normal_x * collision.penetration * 0.5f;
    p1.position.y += collision.normal_y * collision.penetration * 0.5f;
    p2.position.x += -1.f * (collision.normal_x * collision.penetration * 0.5f);
    p2.position.y += -1.f * (collision.normal_y * collision.penetration * 0.5f);

    float rel_vel_x = p1.velocity.x - p2.velocity.x;
    float rel_vel_y = p1.velocity.y - p2.velocity.y;

    float vel_along_normal = rel_vel_x * collision.normal_x +
                            rel_vel_y * collision.normal_y;

    if (vel_along_normal < 0.0f) return;

    float restitution = std::min(p1.damping, p2.damping);
    float j = -(1.0f + restitution) * vel_along_normal / 2.0f;

    float impulse_x = j * collision.normal_x;
    float impulse_y = j * collision.normal_y;

    p1.velocity.x += impulse_x;
    p1.velocity.y += impulse_y;
    p2.velocity.x -= impulse_x;
    p2.velocity.y -= impulse_y;
}

void render_particle(ImDrawList* draw_list, const Particle& p) {
    ImVec2 center(p.position.x, p.position.y);
    draw_list->AddCircleFilled(center, p.radius, IM_COL32(186, 222, 238, 255));
}

void render_boundary(ImDrawList* draw_list, const Boundary& boundary) {
    if (boundary.points.size() < 2) return;

    std::vector<ImVec2> pts;
    for (const auto& p : boundary.points) {
        pts.push_back(ImVec2(p.x, p.y));
    }

    draw_list->AddPolyline(pts.data(), pts.size(),
                           IM_COL32(255, 255, 255, 255),
                           ImDrawFlags_Closed, 2.0f);
}

int main(int argc, char const* argv[]) {
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 1000, "Physics Engine 2", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    SimulationState state;
    state.is_playing = false;
    state.delta_time = 0.016f;

    // Create scene data
    {
        Boundary boundary;
        boundary.points = {
            Vec2(200, 200), Vec2(200, 800),
            Vec2(800, 800), Vec2(800, 200),
            Vec2(200, 200)
        };
        boundary.barycenter = calculate_barycenter(boundary.points);
        state.boundaries.push_back(boundary);
    }

	auto point_at = [](float x, float y, float gravity = 10.f) {
		Particle p;
		p.position = Vec2(x, y);
    	p.initial_position = p.position;
		p.velocity = Vec2(0, 0);
		p.gravity = gravity;
		p.radius = 20.f;
    	p.damping = 0.8f;

		return p;
	};

	float triangle_height = 5;
	float row_spacing = 50;
	float elem_radius = 20;
	float elem_diameter = 2 * elem_radius;
	float x_offset = 100;
	float y_offset = 300;

	int N = triangle_height;

	float gap_entre_base = 30;
	float gap_lateral_base = gap_entre_base / 2;

	float bottom_width = N * (elem_diameter + gap_entre_base);
	float center_x = 100 + bottom_width / 2;

	for (int row = 0; row < triangle_height; row++) {
		int elems_per_row = row + 1;
		float y = row * row_spacing + y_offset;
		float row_w = elems_per_row * (elem_diameter + gap_entre_base);
		float x_start = (center_x - row_w / 2 + gap_lateral_base + elem_radius) + x_offset;

		for (int elem = 0; elem < elems_per_row; elem++) {
			float x = x_start + elem * (elem_diameter + gap_entre_base);
			float gravity = (1.f - std::min(y, 1000.f) / 1000.f) * 10.f;
			state.particles.push_back(point_at(x, y, gravity));
		}
	}

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGui::Begin("Scene", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus);

        // UI Controls
        if (ImGui::Button("Play")) {
            state.is_playing = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            state.is_playing = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            state.is_playing = false;
            for (auto& p : state.particles) {
                p.position = p.initial_position;
                p.velocity = Vec2(0, 0);
                p.is_resting = false;
            }
        }

        // Inspector panel
        if (ImGui::TreeNode("Particles")) {
            for (size_t i = 0; i < state.particles.size(); i++) {
                auto& p = state.particles[i];
                if (ImGui::TreeNode(("Particle " + std::to_string(i)).c_str())) {
                    ImGui::SliderFloat(("Radius##" + std::to_string(i)).c_str(), &p.radius, 5, 50);
                    ImGui::SliderFloat(("Gravity##" + std::to_string(i)).c_str(), &p.gravity, -20, 20);
                    ImGui::SliderFloat(("Damping##" + std::to_string(i)).c_str(), &p.damping, 0, 1);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        if (state.is_playing) {
            state.delta_time = ImGui::GetIO().DeltaTime;

            // Update particles
            for (auto& p : state.particles) {
                update_particle(p, state.delta_time);
            }

            // Check collisions with boundaries
            for (auto& p : state.particles) {
                for (const auto& boundary : state.boundaries) {
                    auto collision = detect_sphere_polyline_collision(
                        p.position, p.radius, boundary.points);
                    resolve_particle_boundary_collision(p, collision);
                }
            }

            // Check particle-particle collisions
            for (size_t i = 0; i < state.particles.size(); i++) {
                for (size_t j = i + 1; j < state.particles.size(); j++) {
                    auto collision = detect_sphere_sphere_collision(
                        state.particles[i].position, state.particles[i].radius,
                        state.particles[j].position, state.particles[j].radius,
                        j);
                    resolve_particle_particle_collision(
                        state.particles[i], state.particles[j], collision);
                }
            }
        }

        for (const auto& boundary : state.boundaries) {
            render_boundary(draw_list, boundary);
        }

        for (const auto& p : state.particles) {
            render_particle(draw_list, p);
        }

        ImGui::End();
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
