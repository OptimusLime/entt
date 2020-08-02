#define CR_HOST

#include <cr.h>
#include <gtest/gtest.h>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>
// #include "types.h"

#include <entt/core/attribute.h>
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include "main_types.h"

#include "types_a.h"

// ENTT_API void update_position(entt::registry &);
// ENTT_API void emplace_velocity(entt::registry &);

TEST(Lib, Multi)
{

    ASSERT_FALSE(entt::resolve_id("position"_hs));

    userdata ud{};
    ud.registry.create();

    std::vector<std::shared_ptr<cr_plugin>> plugin_contexts;

    // add plugin A and B
    {
        auto ctx = std::make_shared<cr_plugin>();
        ctx->userdata = &ud;
        cr_plugin_load(*ctx, PLUGINA);
        plugin_contexts.emplace_back(std::move(ctx));
    }

    {
        auto ctx = std::make_shared<cr_plugin>();
        ctx->userdata = &ud;
        cr_plugin_load(*ctx, PLUGINB);
        plugin_contexts.emplace_back(std::move(ctx));
    }

    for (auto &ctx : plugin_contexts)
    {
        cr_plugin_update(*ctx);
    }

    // cr_plugin ctx;
    // cr_plugin_load(ctx, PLUGINA);
    // cr_plugin_load(ctx, PLUGINB);
    // cr_plugin_update(ctx);

    entt::meta<double>().conv<int>();

    ASSERT_TRUE(entt::resolve_id("position"_hs));
    ASSERT_TRUE(entt::resolve_id("velocity"_hs));
    ASSERT_TRUE(entt::resolve_id("movement"_hs));

    auto entity = ud.registry.create();
    double def_friction = 1.0;

    auto pos = entt::resolve_id("position"_hs).construct(42., 3.);
    auto vel = entt::resolve_id("velocity"_hs).ctor<>().invoke();
    auto move = entt::resolve_id("movement"_hs).construct(def_friction);
    // ASSERT_TRUE(pos && vel);
    ASSERT_TRUE(pos && vel && move);

    // let us update the velocity
    vel.type().data("dx"_hs).set(vel, 1.0);
    vel.type().data("dy"_hs).set(vel, 1.0);

    // we will add to the registry
    pos.type().func("add_to_registry"_hs).invoke(pos, pos, entity, &ud.registry);
    vel.type().func("add_to_registry"_hs).invoke(vel, vel, entity, &ud.registry);

    // auto e2 = ud.registry.create();
    move.type().func("add_to_registry"_hs).invoke(move, move, entity, &ud.registry);

    ASSERT_EQ(pos.type().data("x"_hs).type(), entt::resolve<int>());
    ASSERT_TRUE(pos.type().data("y"_hs).get(pos).try_cast<int>());
    ASSERT_EQ(pos.type().data("x"_hs).get(pos).cast<int>(), 42);
    ASSERT_EQ(pos.type().data("y"_hs).get(pos).cast<int>(), 3);

    ASSERT_EQ(vel.type().data("dx"_hs).type(), entt::resolve<double>());
    ASSERT_TRUE(vel.type().data("dy"_hs).get(vel).convert<double>());
    // ASSERT_EQ(vel.type().data("dx"_hs).get(vel).cast<double>(), 0.);
    // ASSERT_EQ(vel.type().data("dy"_hs).get(vel).cast<double>(), 0.);
    ASSERT_EQ(vel.type().data("dx"_hs).get(vel).cast<double>(), 1.);
    ASSERT_EQ(vel.type().data("dy"_hs).get(vel).cast<double>(), 1.);

    // ASSERT_EQ(move.type().data("friction"_hs).type(), entt::resolve<double>());
    // ASSERT_EQ(move.type().data("friction"_hs).get(move).cast<double>(), .99);

    // ASSERT_EQ(ud.any.type(), entt::resolve<int>());
    // ASSERT_EQ(ud.any.cast<int>(), 42);

    //run again, this time we've moved due to velocy + movement
    for (auto &ctx : plugin_contexts)
    {
        cr_plugin_update(*ctx);
    }

    // move.type().func("remove_from_registry"_hs).invoke(move, move, entity, &ud.registry);
    auto &local_pos = ud.registry.get<position>(entity);
    ASSERT_EQ(local_pos.x, 42 + 16);
    ASSERT_EQ(local_pos.y, 3 + 16);
    auto &local_vel = ud.registry.get<velocity>(entity);
    ASSERT_EQ(local_vel.dx, 1. * def_friction);
    ASSERT_EQ(local_vel.dy, 1. * def_friction);

    // these objects have been initialized from a different context
    ud.registry.clear();
    pos.emplace<void>();
    vel.emplace<void>();
    move.emplace<void>();
    ud.any.emplace<void>();

    // ud.registry.clear()

    ud.registry = {};

    for (auto &ctx : plugin_contexts)
    {
        cr_plugin_close(*ctx);
    }

    ASSERT_FALSE(entt::resolve_id("position"_hs));
    ASSERT_FALSE(entt::resolve_id("velocity"_hs));
    // ASSERT_FALSE(entt::resolve_id("movement"_hs));
    // entt::registry registry;

    // for(auto i = 0; i < 3; ++i) {
    //     const auto entity = registry.create();
    //     registry.emplace<position>(entity, i, i);
    // }

    // emplace_velocity(registry);
    // update_position(registry);

    // ASSERT_EQ(registry.size<position>(), registry.size<velocity>());

    // registry.view<position>().each([](auto entity, auto &position) {
    //     ASSERT_EQ(position.x, entt::to_integral(entity) + 16);
    //     ASSERT_EQ(position.y, entt::to_integral(entity) + 16);
    // });
}
