# RadioMusic Kick Handoff

This project is for the MTM Radio Music hardware and Radio Music panel language. Do not frame the work as Chord Organ.

## Goal

Build a pure kick module: no sample drums, no multi-drum behavior, no generic drum voice. The first engine must be an excellent dedicated kick voice with deep, musical, boomy decay.

The final direction is exactly four separate kick synthesis engines, switchable with the button. This is not optional or a maybe. The workflow is:

1. Keep one excellent pure kick engine stable.
2. Make Start CV accent work musically.
3. Add separate switchable kick engines one at a time.
4. End with four independent kick synth paths.

## Current Control Model

- Station knob: tune offset.
- Station CV: pitch CV into the kick engine.
- Start knob: decay.
- Trigger gate length: extends decay/boom naturally.
- Page 2 Station: pitch envelope.
- Page 2 Start: click.
- Start CV: accent.

Use Radio Music panel names. "Hardness" should be called "click" in user-facing docs and discussion.

## Accent Direction

Accent should not be a simple volume boost. The desired direction is saturation/drive-like accent that feels integrated with the kick and follows the decay/tail naturally.

Current listening result: the present accent build is not right. The accent does not follow the decay tail properly and it clips/cuts out. Do not treat the current accent implementation as solved.

Next accent work should restart from the stable boom baseline, preserve that hex first, then add accent in the smallest possible step. The accent needs to saturate the body/tail naturally without breaking the long decay.

Avoid clever, hidden, or broad rewrites. Change one thing at a time and preserve a testable hex before experiments.

## Known Fork Context

The failed previous attempt fork is:

https://github.com/nymphnerds/RadioMusic-kick

That fork had an attempted kick-only direction and another kick synthesis engine idea, but the work became unreliable due to rushed fixes, poor communication, and losing track of known-good versions. Future work should mine useful ideas from it only after the current single kick engine is stable.

## Do Not Repeat

- Do not reintroduce sample drum paths.
- Do not touch unrelated Chord Organ work.
- Do not add poly/tail-overlap behavior unless explicitly restarted as a separate experiment.
- Do not make large refactors during sound-design testing.
- Do not overwrite a good hex without saving and naming it first.
- Do not put session-history language in the README.

## Current Local State

Current branch: `accent-saturation`.

Current built hex:

`MultiDrums.hex`

SHA-256:

`414d5b08ce28fb8640242c47e5017c08b44d8eca5ea41ad4fa53078b0793e97a`

This current accent build failed listening tests for the accent tail behavior. Return to the saved `boombaseline` branch first, then add accent again in the smallest possible step.
