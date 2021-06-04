# &#x269B; Nucleus
Nucleus is a bytecode driven programming language initially developed to be a learning experience for myself in creating my own language from scratch. Much of the internal workings of Nucleus stems from the [crafting-interpreters](https://craftinginterpreters.com/) book, which has been an excellent resource for building a solid foundation for Nucleus. 

The main goals, a solidly reliable scripting language that aims to have a (decently) rich native library, break some of the norms of traditional language lexicology, define better interoperability between dynamic typings, and be easily accesible. More on these goals can be found below.

## Quick Start
Currently Nucleus is still in development and as such is only accessible through manually building.

## Primary Goals
- Solid, reliable scripting language.
- Rich (decently) native library.
- Break some traditional norms for lexicology and some syntax.
- Define better interoperability between dynamic typings.
- Easily accesible.

## Roadmap
#### Language Features
| Goal | Status | Notes |
|:-:|:-:|-|
| Primatives | Complete | [See Here]() |
| Control | Complete | [See Here]() |
| Reactions | Complete | [See Here]() |
| Models | In Progress | Currently implementing superclasses and base model declaration. |
| Modifiers | In Progress | At this stage only `const` has been implemented, but needs some further clarification to use. |
| Directives | To Do | Only <code>@<span>mutate</span></code> has been implemented to allow for `const` variable mutation.  |
| Standard Library | Ongoing | For now simple items have been created for ease of testing Nucleus. |
| Math Library | To Do | These methods will aim to implement most typical math functionality as expected, but also extra if possible. |
| Primatives/Model Methods | To Do | These will be methods that can be invoked by primatives/models directly relating to their base particle. |

#### Accessibility
| Goal | Status | Notes |
|:-:|:-:|-|
| Node Addon | To Do | This will be implementing the runtime and compiler into a Node Addon. This is primarily a stepping stone to a simple CLI and bundlable executable. |
| CLI | To Do | Will be able to conduct a REPL, and run Nucleus files from the command line. |
| Cross-Platform | To Do | Since this project is being written in C, the aim is to have the runtime compilable for all devices. Additionally, some small focus will be kept for smaller embedded device implementation. |

## License
See [LICENSE-MIT](https://github.com/electron/electron/blob/main/LICENSE) for details.