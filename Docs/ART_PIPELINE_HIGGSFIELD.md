# Higgsfield concept and previs pipeline

## Purpose

Use Higgsfield to reduce ambiguity before expensive modeling and animation. Outputs are references for anatomy, materials, movement, shot design, and mood.

## Setup

```bash
npm install -g @higgsfield/cli
higgsfield auth login
npx skills add higgsfield-ai/skills
```

Use the official Higgsfield CLI/skills flow supported by the installed version. Do not store credentials in the repository.

## Output stages

### 1. Anatomy lock

Generate neutral, readable views before cinematic imagery. Reject any output that changes limb count, wing attachment, joint direction, tail length, or head proportions between views.

### 2. Material and age study

Explore scale type, membrane thickness, scars, vascularity, and age effects without changing the locked skeleton.

### 3. Motion keyframes

Generate start, contact, and recovery poses for one action. Treat them as animation reference, not as physically verified motion.

### 4. Combat previs

Generate 5–10 second clips to test camera distance, speed perception, silhouettes, breath visibility, and terrain scale.

### 5. Review and naming

Save only selected outputs:

```text
Generated/Concepts/<family>/<concept-id>_<view>_<version>.<ext>
Generated/Previs/<encounter>/<shot-id>_<version>.<ext>
```

Log decisions in `Docs/DECISIONS.md`. Generated media is ignored by Git by default.

## Prompt anatomy lock

Every prompt should state:

- exact family and limb count;
- wing attachment and folding direction;
- leg posture;
- body proportions and scale object;
- life stage and experience cues;
- material response;
- action and weight transfer;
- camera/lens and framing;
- environment and lighting;
- negative constraints.

## Negative constraints template

```text
No extra limbs, no missing limbs, no humanoid hands, no feathered bird anatomy unless explicitly requested, no weightless hovering, no rubbery joints, no duplicated heads, no decorative armor, no saddle, no rider, no text, no watermark, no cropped wings, no inconsistent scale.
```

## Review checklist

- Is the body plan consistent with the taxonomy?
- Could the pose plausibly support the shown mass?
- Is the center of mass supported on the ground?
- Are wing membranes attached consistently?
- Does the action have anticipation, contact, and recovery?
- Is the camera useful for gameplay or only cinematic spectacle?
- Can a modeler and animator extract actionable information?
