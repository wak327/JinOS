# jinOS Android Runtime Stub

The `runtime/` module hosts the integration layer between the jinOS kernel and an
Android application runtime (e.g. Anbox, Waydroid, or a custom container).

## Current Scope

* Initializes a placeholder container context with `runtime_init()`.
* Provides `runtime_launch_android_app()` for starting an Android package.
* Implements `runtime_forward_syscall()` that traces syscalls originating from the
  container and returns an `UNSUPPORTED` status.

No actual Android runtime is bundled yet; all interactions are stubbed out and
designed to be replaced with real integration code.

## Extending the Runtime

1. **Integrate Anbox/Waydroid:**
   * Replace the stub in `runtime_launch_android_app()` with logic that communicates
     with an Anbox session manager.
   * Mount Android images (system, vendor) into a container namespace before
     launching the target activity.

2. **Implement Syscall Mediation:**
   * Map Android/Bionic syscalls to jinOS kernel services by extending
     `runtime_forward_syscall()`.
   * Introduce translation layers for file descriptors, sockets, and process
     management.

3. **IPC and Graphics:**
   * Wire guest Binder/Wayland surfaces to the jinOS compositor.
   * Use shared memory buffers or virtio-gpu when running under virtualization.

4. **Security and Sandboxing:**
   * Consider namespacing, SELinux policies, or container capabilities to isolate
     Android apps from native jinOS processes.

## Building

The kernel build system links `runtime/container.c` directly into the kernel image.
Future iterations may move the runtime into a loadable module or dedicated
userspace service.

## Testing

`runtime_forward_syscall()` currently emits trace logs when invoked. Extend the
existing scheduler tasks or user-space utilities to call into the runtime for
experimentation.

