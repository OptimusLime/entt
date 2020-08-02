#include <cr.h>
#include <entt/core/hashed_string.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include "types_a.h"
#include "main_types.h"
#include <entt/entity/registry.hpp>

userdata *gCtx;

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

position create_position(int x, int y)
{
    return position{x, y};
}

void iterate_position(entt::registry *io_registry)
{
    io_registry->view<position>().each([](auto &pos) {
        auto xx = pos.x * pos.y;
    });
}

void set_up_a()
{
    entt::meta<position>()
        .type("position"_hs)
        .ctor<&create_position>()
        .func<&AssignComponentFromMetaAny<position>>("add_to_registry"_hs)
        .func<&iterate_position>("iterate_pos"_hs)
        .data<&position::x>("x"_hs)
        .data<&position::y>("y"_hs);

    entt::meta<velocity>()
        .type("velocity"_hs)
        .ctor<>()
        .func<&AssignComponentFromMetaAny<velocity>>("add_to_registry"_hs)
        .data<&velocity::dx>("dx"_hs)
        .data<&velocity::dy>("dy"_hs);
}

void tear_down_a()
{
    entt::meta<position>().reset();
    entt::meta<velocity>().reset();
}

CR_EXPORT int cr_main(cr_plugin *ctx, cr_op operation)
{
    switch (operation)
    {
    case CR_LOAD:
        gCtx = static_cast<userdata *>(ctx->userdata);
        entt::meta_ctx::bind(gCtx->ctx);
        set_up_a();
        break;
    case CR_STEP:
        // static_cast<userdata *>(ctx->userdata)->any = 42;
        break;
    case CR_UNLOAD:
    case CR_CLOSE:
        tear_down_a();
        gCtx = NULL;
        break;
    }

    return 0;
}
