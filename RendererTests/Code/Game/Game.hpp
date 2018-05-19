#pragma once

class Game {
public:
    Game() = default;
    ~Game() = default;

    void Initialize();
    void BeginFrame();
    void Update(float deltaSeconds);
    void Render() const;
    void EndFrame();

protected:
private:

};