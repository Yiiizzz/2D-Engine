#pragma once

class Timestep {
public:
    explicit Timestep(float timeInSeconds = 0.0f)
        : m_TimeInSeconds(timeInSeconds) {
    }

    float GetSeconds() const {
        return m_TimeInSeconds;
    }

    float GetMilliseconds() const {
        return m_TimeInSeconds * 1000.0f;
    }

    operator float() const {
        return m_TimeInSeconds;
    }

private:
    float m_TimeInSeconds = 0.0f;
};
