#!/bin/bash
podman run \
	--network=host \
	--rm \
	-v $(pwd):/code \
	-it \
	cpp-pod \
	ash
