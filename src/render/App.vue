<template>
    <h1>Application Monitor</h1>
    <p>
        <template v-if="!hasReceivedApplications">
            Waiting for application scanner to return results
        </template>
        <template v-else-if="!hasScannedApplications">
            No scanned applications received
        </template>
        <template v-else>
            <label for="appSelector">Choose an application:</label>
            <select id="appSelector" v-model="selectedApplication" @change="onApplicationChange">
                <option v-for="app in scannedApplications" :key="app.procedureName" :value="app">
                    {{ app.windowTitle }}
                </option>
            </select>

            <div v-if="selectedApplication">
                <h2>Monitoring: {{ selectedApplication.procedureName }}</h2>
                <p><strong>Window Title:</strong> {{ selectedApplication.windowTitle }}</p>
                <p><strong>Status:</strong> {{ applicationStatus?.status || "Unknown" }}</p>
                <template v-if="applicationStatus?.dimensions">
                    <p><strong>Window Dimensions:</strong></p>
                    <ul>
                        <li>X: {{ applicationStatus.dimensions.x }}</li>
                        <li>Y: {{ applicationStatus.dimensions.y }}</li>
                        <li>Width: {{ applicationStatus.dimensions.width }}</li>
                        <li>Height: {{ applicationStatus.dimensions.height }}</li>
                    </ul>
                </template>
            </div>
        </template>
    </p>
</template>

<script setup lang="ts">
    import { computed, onMounted, ref} from 'vue';

    const hasReceivedApplications = ref(false);
    const scannedApplications = ref<ScannedApplications>([]);
    const hasScannedApplications = computed(() => scannedApplications.value.length > 0);

    const selectedApplication = ref<ApplicationInfo | null>(null);
    const applicationStatus = ref<ApplicationStatus | null>(null);

    onMounted(() => {
        console.log("Vue Mounted: Starting application scanner...");

        window.ApplicationMonitorApi.StartApplicationScanner((newScannedApplications) => {
            console.log("Applications Found:", newScannedApplications);
            hasReceivedApplications.value = true;
            scannedApplications.value = newScannedApplications;

            if (newScannedApplications.length > 0) {
                selectedApplication.value = newScannedApplications[0];
                startMonitoring(selectedApplication.value);
            }
        });
    });

    const onApplicationChange = () => {
        console.log("onApplicationChange triggered:", selectedApplication.value);
        if (selectedApplication.value) {
            startMonitoring(selectedApplication.value);
        }
    };

    // Stop previous monitoring & start monitoring new app
    const startMonitoring = (app: ApplicationInfo) => {
        console.log("Stopping previous monitoring...");
        window.ApplicationMonitorApi.StopApplicationStatusMonitor();

        console.log("Starting monitoring for:", app);
        window.ApplicationMonitorApi.StartApplicationStatusMonitor(
            { applicationName: app.procedureName, windowTitle: app.windowTitle },
            (status) => {
                console.log("Status update received:", status);
                applicationStatus.value = status; // Vue will now reactively update UI
            }
        );
    };
</script>

<style lang="css" scoped>
    li {
        line-height: 1.5rem;
    }
        li + li {
            padding-top: 1rem;
        }
</style>
