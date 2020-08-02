#include <cr.h>
#include <entt/core/hashed_string.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include "types_a.h"
#include "types_b.h"
#include "main_types.h"
#include <entt/core/type_info.hpp>
#include <entt/entity/registry.hpp>

template <class TComponent>
void AssignComponentFromMetaAny(
    TComponent const &i_component_instance,
    entt::entity i_entity,
    entt::registry *io_registry)
{
    assert(io_registry);
    assert(io_registry->valid(i_entity));

    io_registry->emplace<TComponent>(i_entity, i_component_instance);
}

template <class TComponent>
void RemoveComponentFromMetaAny(
    TComponent const &i_component_instance,
    entt::entity i_entity,
    entt::registry *io_registry)
{
    assert(io_registry);
    assert(io_registry->valid(i_entity));

    io_registry->remove<TComponent>(i_entity);
}

movement create_movement(double friction)
{
    return movement{friction};
}

void set_up_b()
{
    entt::meta<movement>()
        .type("movement"_hs)
        // .ctor<>()
        .ctor<&create_movement>()
        .func<&AssignComponentFromMetaAny<movement>>("add_to_registry"_hs)
        .func<&RemoveComponentFromMetaAny<movement>>("remove_from_registry"_hs)
        .data<&movement::friction>("friction"_hs);
}

void tear_down_b()
{
    entt::meta<movement>().reset();
}

void step_b(cr_plugin *ctx)
{
    auto &registry = static_cast<userdata *>(ctx->userdata)->registry;
    // static_cast<userdata *>(ctx->userdata)->any = 42;

    // auto &registry = *static_cast<entt::registry *>(ctx->userdata);

    // const auto position_view = registry.view<position>(entt::exclude<tag>);
    // registry.insert(position_view.begin(), position_view.end(), velocity{1., 1.});
    // registry.view<position, velocity>().each([](auto &pos, auto &vel) {
    //     pos.x += static_cast<int>(16 * vel.dx);
    //     pos.y += static_cast<int>(16 * vel.dy);
    // });

    // registry.view<movement>().each([](auto &move) {
    //     move.friction = 0;
    // });

    registry.view<position, velocity, movement>().each([](auto &pos, auto &vel, auto &move) {
        pos.x += static_cast<int>(16 * vel.dx);
        pos.y += static_cast<int>(16 * vel.dy);

        // slow down next movement because we are friction type
        vel.dx *= move.friction;
        vel.dy *= move.friction;
    });

    // const auto tag_view = registry.view<tag>();
    // registry.destroy(tag_view.begin(), tag_view.end());
}

CR_EXPORT int cr_main(cr_plugin *ctx, cr_op operation)
{
    switch (operation)
    {
    case CR_LOAD:
        entt::meta_ctx::bind(static_cast<userdata *>(ctx->userdata)->ctx);
        set_up_b();
        break;
    case CR_STEP:
        // need to load all registry objects
        step_b(ctx);
        break;
    case CR_UNLOAD:
    case CR_CLOSE:
        tear_down_b();
        break;
    }

    return 0;
}
