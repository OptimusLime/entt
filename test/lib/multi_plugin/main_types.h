#pragma once
#include <entt/meta/meta.hpp>
// #include <entt/core/attribute.h>
#include <entt/entity/registry.hpp>

struct userdata
{
    entt::meta_ctx ctx;
    entt::meta_any any;
    entt::registry registry;
};
