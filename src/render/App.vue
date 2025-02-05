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

            <!-- Polling Time Input -->
            <div>
                <label for="pollingTimeId"><strong>Set Polling Time (ms):</strong></label>
                <input type="number"
                       id="pollingTimeId"
                       v-model="pollingTime"
                       :min="minPollingTime"
                       :max="maxPollingTime"
                       @input="validatePollingTime" />
                <button @click="submitPollingTime">
                    Submit
                </button>
                <p v-if="pollingError" class="error-message">
                    Polling time must be between {{ minPollingTime }} and {{ maxPollingTime }} ms.
                </p>
            </div>

            <!-- Application Status -->
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
    import { computed, onMounted, ref } from 'vue';

    const hasReceivedApplications = ref(false);
    const scannedApplications = ref<ScannedApplications>([]);
    const hasScannedApplications = computed(() => scannedApplications.value.length > 0);

    const selectedApplication = ref<ApplicationInfo | null>(null);
    const applicationStatus = ref<ApplicationStatus | null>(null);

    // Polling Time State
    const pollingTime = ref(3000);
    const minPollingTime = 500;
    const maxPollingTime = 10000;
    const pollingError = ref(false);

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
        if (selectedApplication.value) {
            startMonitoring(selectedApplication.value);
        }
    };

    const startMonitoring = (app: ApplicationInfo) => {
        window.ApplicationMonitorApi.StopApplicationStatusMonitor();
        //Set a small timeout for threads switching
        setTimeout(() => {
            window.ApplicationMonitorApi.StartApplicationStatusMonitor(
                { applicationName: app.procedureName, windowTitle: app.windowTitle },
                (status) => {
                    applicationStatus.value = status;
                }
            );
        }, 100);
    };

    // Validate polling time input
    const validatePollingTime = () => {
        pollingError.value =
            pollingTime.value < minPollingTime || pollingTime.value > maxPollingTime;
    };

    // Submit polling time if valid
    const submitPollingTime = () => {
        validatePollingTime();
        if (!pollingError.value) {

            window.ApplicationMonitorApi.SetPollingTime(pollingTime.value, (updatedTime) => {
                pollingTime.value = updatedTime;
            });
        }
    };
</script>

<style lang="css" scoped>
    li {
        line-height: 1.5rem;
    }

    li + li {
        padding-top: 1rem;
    }

    .error-message {
        color: red;
        font-weight: bold;
        margin-top: 5px;
    }
</style>
