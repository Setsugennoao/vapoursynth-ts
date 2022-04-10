# TypeScript bindings for VapourSynth

## Installation

```sh
yarn add vapoursynth-ts
```
<br>

## Getting started

```ts
import { readFileSync } from 'fs'
import { core, PyScript } from 'vapoursynth-ts'

// Like you would in the Python bindings
const source = core.lsmas.LibavSMASHSource('./haruhi_01.mkv')

// can also import a Python script
const myscript = new PyScript(readFileSync('./haruhi_01.vpy'))
const myscript2 = new PyScript(readFileSync('./haruhi_01v2.vpy'))

// Boring!!
const comp = core.std.StackVertical([
    myscript.getOutput(0),
    myscript2.getOutput(0)
])

// Way better 👍
const bitOfGrain = comp.grain.Add(10, 15)

// Way more betterer 👍👍
const lottaOfGrain = comp.grain.Add({ var: 26, uvar: 12 })

source.setOutput(0)
comp.setOutput(1)
bitOfGrain.setOutput(2)
lottaOfGrain.setOutput(3)

```
<br>

## Specifying core flags

```ts
import { core, CoreCreationFlags, setCoreCreationFlags } from 'vapoursynth-ts'

// Has to be set before any call to core has been done
setCoreCreationFlags(
    CoreCreationFlags.DisableAutoLoading,
    CoreCreationFlags.DisableLibraryUnloading,
    CoreCreationFlags.EnableGraphInspection
)

const clip = core.std.BlankClip()
```
