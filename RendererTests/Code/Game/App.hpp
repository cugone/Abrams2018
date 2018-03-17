#pragma once

class App {
public:
    App();
    ~App();

    bool IsQuitting() const;
    void SetIsQuitting(bool quit);
    void Initialize();

    void RunFrame();

protected:
private:
    void BeginFrame();
    void Update(float deltaSeconds);
    void Render() const;
    void EndFrame();

    bool _isQuitting = false;

};