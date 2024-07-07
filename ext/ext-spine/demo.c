/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated July 28, 2023. Replaces all prior versions.
 *
 * Copyright (c) 2013-2023, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software or
 * otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THE
 * SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include "engine.h"

#undef MALLOC
#undef REALLOC
#undef FREE
#undef CALLOC
//#define UNUSED(x) ((void)(x))

#include <spine/Debug.h>
#include <spine/extension.h>


#if 1

void _spAtlasPage_createTexture(spAtlasPage *self, const char *path) {
	unsigned flags = 0;

	if (self->magFilter == SP_ATLAS_LINEAR) flags |= TEXTURE_LINEAR;
	if (self->uWrap == SP_ATLAS_REPEAT && self->vWrap == SP_ATLAS_REPEAT) flags |= TEXTURE_REPEAT;

	texture_t *t = malloc(sizeof(texture_t));
	*t = texture(path, flags);

	self->rendererObject = texture;
	self->width = t->w;
	self->height = t->h;
}

void _spAtlasPage_disposeTexture(spAtlasPage *self) {
	texture_t* t = (texture_t*)self->rendererObject;
	texture_destroy( t );
	free( t );
}

char *_spUtil_readFile(const char *path, int *length) {
	return file_read(path, length);
}

#endif

#if 1

#ifndef SPINE_MESH_VERTEX_COUNT_MAX
#define SPINE_MESH_VERTEX_COUNT_MAX 1000
#endif

typedef struct spVertex {
	vec2 position;
	vec2 texCoords;
	struct { uint8_t r,g,b,a; } color;
} spVertex;

typedef struct SkeletonDrawable {
	//public:
	spSkeleton *skeleton;
	spAnimationState *state;
	spAnimationStateData *state_data;
	float timeScale;

	//private:
	bool ownsAnimationStateData;
	array(float) worldVertices;
	array(spVertex) vertexArray;
	spSkeletonClipping *clipper;
	bool usePremultipliedAlpha;
} SkeletonDrawable;


	SkeletonDrawable *SkeletonDrawable_new(spSkeletonData *skeletonData, spAnimationStateData *stateData) {
		SkeletonDrawable *self = calloc(1, sizeof(SkeletonDrawable));
		self->timeScale = 1;
		array_resize(self->vertexArray, 3 * skeletonData->bonesCount * 4);

		spBone_setYDown(true);
		array_resize(self->worldVertices, SPINE_MESH_VERTEX_COUNT_MAX);

		self->skeleton = spSkeleton_create(skeletonData);
		self->ownsAnimationStateData = stateData == 0;
		if (self->ownsAnimationStateData) stateData = spAnimationStateData_create(skeletonData);

		self->state = spAnimationState_create(self->state_data = stateData);

		self->clipper = spSkeletonClipping_create();

		return self;
	}

	void SkeletonDrawable_delete(SkeletonDrawable *self) {
		array_free(self->vertexArray);
		array_free(self->worldVertices);
		if (self->ownsAnimationStateData) spAnimationStateData_dispose(self->state_data);
		spAnimationState_dispose(self->state);
		spSkeleton_dispose(self->skeleton);
		spSkeletonClipping_dispose(self->clipper);

		free(self);
	}

	void SkeletonDrawable_tick(SkeletonDrawable *self, float deltaTime, spPhysics physics) {
		spAnimationState_update(self->state, deltaTime * self->timeScale);
		spAnimationState_apply(self->state, self->skeleton);
		spSkeleton_update(self->skeleton, deltaTime * self->timeScale);
		spSkeleton_updateWorldTransform(self->skeleton, physics);
	}

	struct RenderTarget {
		int dummy;
	};

	struct RenderStates {
		texture_t *texture;
		int blendMode;
	};

	// non premultiplied alpha
	#define BLENDMODE(a,b) (((a)<<16)|(b))
	#define BLENDMODE_NORMAL BLENDMODE(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	#define BLENDMODE_ADDITIVE BLENDMODE(GL_SRC_ALPHA, GL_ONE);
	#define BLENDMODE_MULTIPLY BLENDMODE(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	#define BLENDMODE_SCREEN BLENDMODE(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
	// premultiplied alpha
	#define BLENDMODE_NORMAL_PMA BLENDMODE(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	#define BLENDMODE_ADDITIVE_PMA BLENDMODE(GL_ONE, GL_ONE);
	#define BLENDMODE_MULTIPLY_PMA BLENDMODE(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	#define BLENDMODE_SCREEN_PMA BLENDMODE(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

	void rendertarget_draw(struct RenderTarget *target, const struct RenderStates *states, array(spVertex) vertexArray) {

	}

	void SkeletonDrawable_draw(SkeletonDrawable *self, struct RenderTarget *target, struct RenderStates *states) {
		array_resize(self->vertexArray, 0);

		states->texture = 0;
		unsigned short quadIndices[6] = {0, 1, 2, 2, 3, 0};

		// Early out if skeleton is invisible
		if (self->skeleton->color.a == 0) return;

		spVertex vertex = {0};
		texture_t *texture = 0;
		for (int i = 0; i < self->skeleton->slotsCount; ++i) {
			spSlot *slot = self->skeleton->drawOrder[i];
			spAttachment *attachment = slot->attachment;
			if (!attachment) {
				spSkeletonClipping_clipEnd(self->clipper, slot);
				continue;
			}

			// Early out if slot is invisible
			if (slot->color.a == 0 || !slot->bone->active) {
				spSkeletonClipping_clipEnd(self->clipper, slot);
				continue;
			}

			float *vertices = self->worldVertices;
			int verticesCount = 0;
			float *uvs = 0;
			unsigned short *indices = 0;
			int indicesCount = 0;
			spColor *attachmentColor;

			if (attachment->type == SP_ATTACHMENT_REGION) {
				spRegionAttachment *regionAttachment = (spRegionAttachment *) attachment;
				attachmentColor = &regionAttachment->color;

				// Early out if slot is invisible
				if (attachmentColor->a == 0) {
					spSkeletonClipping_clipEnd(self->clipper, slot);
					continue;
				}

				spRegionAttachment_computeWorldVertices(regionAttachment, slot, vertices, 0, 2);
				verticesCount = 4;
				uvs = regionAttachment->uvs;
				indices = quadIndices;
				indicesCount = 6;
				texture = (texture_t *) ((spAtlasRegion *) regionAttachment->rendererObject)->page->rendererObject;

			} else if (attachment->type == SP_ATTACHMENT_MESH) {
				spMeshAttachment *mesh = (spMeshAttachment *) attachment;
				attachmentColor = &mesh->color;

				// Early out if slot is invisible
				if (attachmentColor->a == 0) {
					spSkeletonClipping_clipEnd(self->clipper, slot);
					continue;
				}

				if (mesh->super.worldVerticesLength > SPINE_MESH_VERTEX_COUNT_MAX) continue;
				spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength, self->worldVertices, 0, 2);
				verticesCount = mesh->super.worldVerticesLength >> 1;
				uvs = mesh->uvs;
				indices = mesh->triangles;
				indicesCount = mesh->trianglesCount;
				texture = (texture_t *) ((spAtlasRegion *) mesh->rendererObject)->page->rendererObject;

			} else if (attachment->type == SP_ATTACHMENT_CLIPPING) {
				spClippingAttachment *clip = (spClippingAttachment *) slot->attachment;
				spSkeletonClipping_clipStart(self->clipper, slot, clip);
				continue;
			} else
				continue;

			uint8_t r = (uint8_t)(self->skeleton->color.r * slot->color.r * attachmentColor->r * 255);
			uint8_t g = (uint8_t)(self->skeleton->color.g * slot->color.g * attachmentColor->g * 255);
			uint8_t b = (uint8_t)(self->skeleton->color.b * slot->color.b * attachmentColor->b * 255);
			uint8_t a = (uint8_t)(self->skeleton->color.a * slot->color.a * attachmentColor->a * 255);
			vertex.color.r = r;
			vertex.color.g = g;
			vertex.color.b = b;
			vertex.color.a = a;

			spColor light;
			light.r = r / 255.0f;
			light.g = g / 255.0f;
			light.b = b / 255.0f;
			light.a = a / 255.0f;

			int blend;
			if (!self->usePremultipliedAlpha) {
				switch (slot->data->blendMode) {
					case SP_BLEND_MODE_NORMAL:
						blend = BLENDMODE_NORMAL;
						break;
					case SP_BLEND_MODE_ADDITIVE:
						blend = BLENDMODE_ADDITIVE;
						break;
					case SP_BLEND_MODE_MULTIPLY:
						blend = BLENDMODE_MULTIPLY;
						break;
					case SP_BLEND_MODE_SCREEN:
						blend = BLENDMODE_SCREEN;
						break;
					default:
						blend = BLENDMODE_NORMAL;
				}
			} else {
				switch (slot->data->blendMode) {
					case SP_BLEND_MODE_NORMAL:
						blend = BLENDMODE_NORMAL_PMA;
						break;
					case SP_BLEND_MODE_ADDITIVE:
						blend = BLENDMODE_ADDITIVE_PMA;
						break;
					case SP_BLEND_MODE_MULTIPLY:
						blend = BLENDMODE_MULTIPLY_PMA;
						break;
					case SP_BLEND_MODE_SCREEN:
						blend = BLENDMODE_SCREEN_PMA;
						break;
					default:
						blend = BLENDMODE_NORMAL_PMA;
				}
			}

			if (states->texture == 0) states->texture = texture;

			if (states->blendMode != blend || states->texture != texture) {
				rendertarget_draw(target, states, self->vertexArray);
				array_resize(self->vertexArray, 0);
				states->blendMode = blend;
				states->texture = texture;
			}

			if (spSkeletonClipping_isClipping(self->clipper)) {
				spSkeletonClipping_clipTriangles(self->clipper, vertices, verticesCount << 1, indices, indicesCount, uvs, 2);
				vertices = self->clipper->clippedVertices->items;
				verticesCount = self->clipper->clippedVertices->size >> 1;
				uvs = self->clipper->clippedUVs->items;
				indices = self->clipper->clippedTriangles->items;
				indicesCount = self->clipper->clippedTriangles->size;
			}

			vec2i size = vec2i( texture->w, texture->h );

			for (int j = 0; j < indicesCount; ++j) {
				int index = indices[j] << 1;
				vertex.position.x = vertices[index];
				vertex.position.y = vertices[index + 1];
				vertex.texCoords.x = uvs[index] * size.x;
				vertex.texCoords.y = uvs[index + 1] * size.y;
				array_push(self->vertexArray, vertex);
			}

			spSkeletonClipping_clipEnd(self->clipper, slot);
		}
		rendertarget_draw(target, states, self->vertexArray);
		spSkeletonClipping_clipEnd2(self->clipper);
	}


#endif


void callback(spAnimationState *state, spEventType type, spTrackEntry *entry, spEvent *event) {
	UNUSED(state);
	const char *animationName = (entry && entry->animation) ? entry->animation->name : 0;

	switch (type) {
		case SP_ANIMATION_START:
			printf("%d start: %s\n", entry->trackIndex, animationName);
			break;
		case SP_ANIMATION_INTERRUPT:
			printf("%d interrupt: %s\n", entry->trackIndex, animationName);
			break;
		case SP_ANIMATION_END:
			printf("%d end: %s\n", entry->trackIndex, animationName);
			break;
		case SP_ANIMATION_COMPLETE:
			printf("%d complete: %s\n", entry->trackIndex, animationName);
			break;
		case SP_ANIMATION_DISPOSE:
			printf("%d dispose: %s\n", entry->trackIndex, animationName);
			break;
		case SP_ANIMATION_EVENT:
			printf("%d event: %s, %s: %d, %f, %s %f %f\n", entry->trackIndex, animationName, event->data->name, event->intValue, event->floatValue,
				   event->stringValue, event->volume, event->balance);
			break;
	}
	fflush(stdout);
}

spSkeletonData *readSkeletonJsonData(const char *filename, spAtlas *atlas, float scale) {
	spSkeletonJson *json = spSkeletonJson_create(atlas);
	json->scale = scale;
	spSkeletonData *skeletonData = spSkeletonJson_readSkeletonDataFile(json, filename);
	if (!skeletonData) {
		printf("%s\n", json->error);
		exit(0);
	}
	spSkeletonJson_dispose(json);
	return skeletonData;
}

spSkeletonData *readSkeletonBinaryData(const char *filename, spAtlas *atlas, float scale) {
	spSkeletonBinary *binary = spSkeletonBinary_create(atlas);
	binary->scale = scale;
	spSkeletonData *skeletonData = spSkeletonBinary_readSkeletonDataFile(binary, filename);
	if (!skeletonData) {
		printf("%s\n", binary->error);
		exit(0);
	}
	spSkeletonBinary_dispose(binary);
	return skeletonData;
}

void testcase(void func(spSkeletonData *skeletonData, spAtlas *atlas),
			  const char *jsonName, const char *binaryName, const char *atlasName,
			  float scale) {
	spAtlas *atlas = spAtlas_createFromFile(atlasName, 0);

	spSkeletonData *skeletonData = readSkeletonJsonData(jsonName, atlas, scale);
	func(skeletonData, atlas);
	spSkeletonData_dispose(skeletonData);

	skeletonData = readSkeletonBinaryData(binaryName, atlas, scale);
	func(skeletonData, atlas);
	spSkeletonData_dispose(skeletonData);

	spAtlas_dispose(atlas);

	UNUSED(jsonName);
}

void spineboy(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);
	spSkeletonBounds *bounds = spSkeletonBounds_create();

	// Configure mixing.
	spAnimationStateData *stateData = spAnimationStateData_create(skeletonData);
	spAnimationStateData_setMixByName(stateData, "walk", "jump", 0.2f);
	spAnimationStateData_setMixByName(stateData, "jump", "run", 0.2f);

	SkeletonDrawable *drawable = SkeletonDrawable_new(skeletonData, stateData);
	drawable->timeScale = 1;
	drawable->usePremultipliedAlpha = true;

	spSkeleton *skeleton = drawable->skeleton;
	spSkeleton_setToSetupPose(skeleton);

	skeleton->x = 320;
	skeleton->y = 590;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);

	spSlot *headSlot = spSkeleton_findSlot(skeleton, "head");

	drawable->state->listener = callback;
	spAnimationState_addAnimationByName(drawable->state, 0, "walk", true, 0);
	spAnimationState_addAnimationByName(drawable->state, 0, "jump", false, 3);
	spAnimationState_addAnimationByName(drawable->state, 0, "run", true, 0);

	while (app_swap()) {

		float delta = 0; // window_delta(); in ss

		spSkeletonBounds_update(bounds, skeleton, true);

#if 0
		vec2 position = { mouse_x(), mouse_y() };
		if (spSkeletonBounds_containsPoint(bounds, (float) position.x, (float) position.y)) {
			headSlot->color.g = 0;
			headSlot->color.b = 0;
		} else {
			headSlot->color.g = 1;
			headSlot->color.b = 1;
		}
#endif
		SkeletonDrawable_tick(drawable, delta, SP_PHYSICS_UPDATE);
		SkeletonDrawable_draw(drawable, NULL, NULL);
	}

	spSkeletonBounds_dispose(bounds);
}

#if 0

void ikDemo(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);

	// Create the SkeletonDrawable and position it
	spAnimationStateData *stateData = spAnimationStateData_create(skeletonData);
	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData, stateData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 320;
	skeleton->y = 590;

	// Queue the "walk" animation on the first track.
	spAnimationState_setAnimationByName(drawable->state, 0, "walk", true);

	// Queue the "aim" animation on a higher track.
	// It consists of a single frame that positions
	// the back arm and gun such that they point at
	// the "crosshair" bone. By setting this
	// animation on a higher track, it overrides
	// any changes to the back arm and gun made
	// by the walk animation, allowing us to
	// mix the two. The mouse position following
	// is performed in the render() method below.
	spAnimationState_setAnimationByName(drawable->state, 1, "aim", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - IK Demo");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;
	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		// Update and apply the animations to the skeleton,
		// then calculate the world transforms of every bone.
		// This is needed so we can call Bone#worldToLocal()
		// later.
		drawable->update(delta, SP_PHYSICS_UPDATE);

		// Position the "crosshair" bone at the mouse
		// location. We do this before calling
		// skeleton.updateWorldTransform() below, so
		// our change is incorporated before the IK
		// constraint is applied.
		//
		// When setting the crosshair bone position
		// to the mouse position, we need to translate
		// from "mouse space" to "local bone space". Note that the local
		// bone space is calculated using the bone's parent
		// worldToLocal() function!
		sf::Vector2i mouseCoords = sf::Mouse::getPosition(window);
		float boneCoordsX = 0, boneCoordsY = 0;
		spBone *crosshair = spSkeleton_findBone(drawable->skeleton, "crosshair");// Should be cached.
		spBone_worldToLocal(crosshair->parent, mouseCoords.x, mouseCoords.y, &boneCoordsX, &boneCoordsY);
		crosshair->x = boneCoordsX;
		crosshair->y = boneCoordsY;

		// Calculate final world transform with the
		// crosshair bone set to the mouse cursor
		// position.
		spSkeleton_updateWorldTransform(drawable->skeleton, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

void goblins(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);
	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	spSkeleton_setSkinByName(skeleton, "goblingirl");
	spSkeleton_setSlotsToSetupPose(skeleton);
	skeleton->x = 320;
	skeleton->y = 590;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);

	spAnimationState_setAnimationByName(drawable->state, 0, "walk", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - goblins");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;
	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

void raptor(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);
	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 320;
	skeleton->y = 590;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);

	spAnimationState_setAnimationByName(drawable->state, 0, "walk", true);
	spAnimationState_addAnimationByName(drawable->state, 1, "gun-grab", false, 2);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - raptor");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;
	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

void tank(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);
	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 500;
	skeleton->y = 590;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);

	spAnimationState_setAnimationByName(drawable->state, 0, "drive", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - tank");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;

	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();
		drawable->update(delta, SP_PHYSICS_UPDATE);
		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

void vine(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);
	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 320;
	skeleton->y = 590;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);

	spAnimationState_setAnimationByName(drawable->state, 0, "grow", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - vine");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;
	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

void stretchyman(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);
	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 100;
	skeleton->y = 590;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);

	spAnimationState_setAnimationByName(drawable->state, 0, "sneak", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - Streatchyman");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;
	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

void coin(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);


	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 320;
	skeleton->y = 320;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);
	spAnimationState_setAnimationByName(drawable->state, 0, "animation", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - vine");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;

	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

void dragon(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);


	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 320;
	skeleton->y = 320;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);
	spAnimationState_setAnimationByName(drawable->state, 0, "flying", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - dragon");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;

	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

void owl(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);
	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 320;
	skeleton->y = 400;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);

	spAnimationState_setAnimationByName(drawable->state, 0, "idle", true);
	spAnimationState_setAnimationByName(drawable->state, 1, "blink", true);
	spTrackEntry *left = spAnimationState_setAnimationByName(drawable->state, 2, "left", true);
	spTrackEntry *right = spAnimationState_setAnimationByName(drawable->state, 3, "right", true);
	spTrackEntry *up = spAnimationState_setAnimationByName(drawable->state, 4, "up", true);
	spTrackEntry *down = spAnimationState_setAnimationByName(drawable->state, 5, "down", true);

	left->alpha = 0;
	left->mixBlend = SP_MIX_BLEND_ADD;
	right->alpha = 0;
	right->mixBlend = SP_MIX_BLEND_ADD;
	up->alpha = 0;
	up->mixBlend = SP_MIX_BLEND_ADD;
	down->alpha = 0;
	down->mixBlend = SP_MIX_BLEND_ADD;

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - owl");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;
	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::MouseMoved) {
				float x = event.mouseMove.x / 640.0f;
				left->alpha = (MAX(x, 0.5f) - 0.5f) * 2;
				right->alpha = (0.5f - MIN(x, 0.5f)) * 2;

				float y = event.mouseMove.y / 640.0f;
				down->alpha = (MAX(y, 0.5f) - 0.5f) * 2;
				up->alpha = (0.5f - MIN(y, 0.5f)) * 2;
			}
		}

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		spSkeleton_setToSetupPose(drawable->skeleton);
		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

/**
 * Used for debugging purposes during runtime development
 */
void test(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);
	spSkeleton *skeleton = spSkeleton_create(skeletonData);
	spAnimationStateData *animData = spAnimationStateData_create(skeletonData);
	spAnimationState *animState = spAnimationState_create(animData);
	spAnimationState_setAnimationByName(animState, 0, "drive", true);


	float d = 3;
	for (int i = 0; i < 1; i++) {
		spAnimationState_update(animState, d);
		spAnimationState_apply(animState, skeleton);
		spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);
		for (int ii = 0; ii < skeleton->bonesCount; ii++) {
			spBone *bone = skeleton->bones[ii];
			printf("%s %f %f %f %f %f %f\n", bone->data->name, bone->a, bone->b, bone->c, bone->d, bone->worldX, bone->worldY);
		}
		printf("========================================\n");
		d += 0.1f;
	}

	spSkeleton_dispose(skeleton);
}

void testSkinsApi(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);
	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;

	spSkin *skin = spSkin_create("test-skin");
	spSkin_copySkin(skin, spSkeletonData_findSkin(skeletonData, "goblingirl"));
	// spSkin_addSkin(skin, spSkeletonData_findSkin(skeletonData, "goblingirl"));
	spSkeleton_setSkin(skeleton, skin);
	spSkeleton_setSlotsToSetupPose(skeleton);

	skeleton->x = 320;
	skeleton->y = 590;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);

	spAnimationState_setAnimationByName(drawable->state, 0, "walk", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - skins api");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;
	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}

	spSkin_clear(skin);
	spSkin_dispose(skin);
}

void testMixAndMatch(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);
	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;

	// Create a new skin, by mixing and matching other skins
	// that fit together. Items making up the girl are individual
	// skins. Using the skin API, a new skin is created which is
	// a combination of all these individual item skins.
	spSkin *skin = spSkin_create("mix-and-match");
	spSkin_addSkin(skin, spSkeletonData_findSkin(skeletonData, "skin-base"));
	spSkin_addSkin(skin, spSkeletonData_findSkin(skeletonData, "nose/short"));
	spSkin_addSkin(skin, spSkeletonData_findSkin(skeletonData, "eyelids/girly"));
	spSkin_addSkin(skin, spSkeletonData_findSkin(skeletonData, "eyes/violet"));
	spSkin_addSkin(skin, spSkeletonData_findSkin(skeletonData, "hair/brown"));
	spSkin_addSkin(skin, spSkeletonData_findSkin(skeletonData, "clothes/hoodie-orange"));
	spSkin_addSkin(skin, spSkeletonData_findSkin(skeletonData, "legs/pants-jeans"));
	spSkin_addSkin(skin, spSkeletonData_findSkin(skeletonData, "accessories/bag"));
	spSkin_addSkin(skin, spSkeletonData_findSkin(skeletonData, "accessories/hat-red-yellow"));
	spSkeleton_setSkin(skeleton, skin);
	spSkeleton_setSlotsToSetupPose(skeleton);

	skeleton->x = 320;
	skeleton->y = 590;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);

	spAnimationState_setAnimationByName(drawable->state, 0, "dance", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - mix and match");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;
	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}

	spSkin_clear(skin);
	spSkin_dispose(skin);
}

void cloudpot(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);

	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 320;
	skeleton->y = 480;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);
	spAnimationState_setAnimationByName(drawable->state, 0, "playing-in-the-rain", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - cloudpot");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;

	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

void sack(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);


	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 320;
	skeleton->y = 480;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);
	spAnimationState_setAnimationByName(drawable->state, 0, "walk", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - sack");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;

	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

void snowglobe(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);


	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 320;
	skeleton->y = 480;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);
	spAnimationState_setAnimationByName(drawable->state, 0, "shake", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - snowglobe");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;

	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

void celestialCircus(spSkeletonData *skeletonData, spAtlas *atlas) {
	UNUSED(atlas);


	SkeletonDrawable *drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;
	drawable->setUsePremultipliedAlpha(true);

	spSkeleton *skeleton = drawable->skeleton;
	skeleton->x = 320;
	skeleton->y = 480;
    skeleton->scaleX = skeleton->scaleY = 0.2f;
	spSkeleton_updateWorldTransform(skeleton, SP_PHYSICS_UPDATE);
	spAnimationState_setAnimationByName(drawable->state, 0, "swing", true);

	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - celestrial circus");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;

	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta, SP_PHYSICS_UPDATE);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
}

int main() {
    testcase(celestialCircus, "data/celestial-circus-pro.json", "data/celestial-circus-pro.skel", "data/celestial-circus-pma.atlas", 1);
	testcase(cloudpot, "data/cloud-pot.json", "data/cloud-pot.skel", "data/cloud-pot-pma.atlas", 0.2);
	testcase(sack, "data/sack-pro.json", "data/sack-pro.skel", "data/sack-pma.atlas", 0.2f);
	testcase(snowglobe, "data/snowglobe-pro.json", "data/snowglobe-pro.skel", "data/snowglobe-pma.atlas", 0.2f);
	testcase(raptor, "data/raptor-pro.json", "data/raptor-pro.skel", "data/raptor-pma.atlas", 0.5f);
	testcase(dragon, "data/dragon-ess.json", "data/dragon-ess.skel", "data/dragon-pma.atlas", 0.6f);
	testcase(ikDemo, "data/spineboy-pro.json", "data/spineboy-pro.skel", "data/spineboy-pma.atlas", 0.6f);
	testcase(spineboy, "data/spineboy-pro.json", "data/spineboy-pro.skel", "data/spineboy-pma.atlas", 0.6f);
	testcase(coin, "data/coin-pro.json", "data/coin-pro.skel", "data/coin-pma.atlas", 0.5f);
	testcase(testMixAndMatch, "data/mix-and-match-pro.json", "data/mix-and-match-pro.skel", "data/mix-and-match-pma.atlas", 0.5f);
	testcase(test, "data/tank-pro.json", "data/tank-pro.skel", "data/tank-pma.atlas", 1.0f);
	testcase(owl, "data/owl-pro.json", "data/owl-pro.skel", "data/owl-pma.atlas", 0.5f);
	testcase(vine, "data/vine-pro.json", "data/vine-pro.skel", "data/vine-pma.atlas", 0.5f);
	testcase(tank, "data/tank-pro.json", "data/tank-pro.skel", "data/tank-pma.atlas", 0.2f);
	testcase(goblins, "data/goblins-pro.json", "data/goblins-pro.skel", "data/goblins-pma.atlas", 1.4f);
	testcase(stretchyman, "data/stretchyman-pro.json", "data/stretchyman-pro.skel", "data/stretchyman-pma.atlas", 0.6f);
	testcase(testSkinsApi, "data/goblins-pro.json", "data/goblins-pro.skel", "data/goblins-pma.atlas", 1.4f);
	return 0;
}

#endif

int main() {
	app_create(0.75, 0);

	testcase(spineboy, "data/spineboy-pro.json", "data/spineboy-pro.skel", "data/spineboy-pma.atlas", 0.6f);
	return 0;
}
