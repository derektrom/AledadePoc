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
          Active applications with windows: {{ scannedApplications.length }}
          <label for="cars">Choose an application:</label>
          <select id="appSeletor ">
              <option v-for="app in scannedApplications" :key="app.procedureName" :value="app">
                  {{ app.windowTitle }}
              </option>
          </select>
      </template>
    </p>
  </template>
  
<script setup lang="ts">
import { computed, onMounted, ref } from 'vue';

const hasReceivedApplications = ref(false);
const scannedApplications = ref<ScannedApplications>([]);
const hasScannedApplications = computed(() => {
  return scannedApplications.value.length > 0;
});
onMounted(() => {
  window.ApplicationMonitorApi.StartApplicationScanner((newScannedApplications) => {
    hasReceivedApplications.value = true;
    scannedApplications.value = newScannedApplications;
  });
});
</script>

<style lang="css" scoped>
li {
  line-height: 1.5rem;
}
li + li {
  padding-top: 1rem;
}
</style>