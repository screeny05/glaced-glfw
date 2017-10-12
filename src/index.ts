import * as bindings from 'bindings';

export const glfw: GLFW = bindings('glfw');

(<any>glfw).inspect = (depth, options) => options.stylize('[object GLFW]', 'special');
