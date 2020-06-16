#pragma once

#include "Core/Core.h"

namespace Vertex
{
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const  = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;
    };

    class Texture2D : public Texture
    {
    public:
        static Texture2D* Create(const char* path);
    };
} // namespace Vertex
