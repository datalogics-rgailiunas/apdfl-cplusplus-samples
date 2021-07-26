def ENV_LOC=[:]
pipeline {
    parameters {
        choice(name: 'PLATFORM_FILTER', choices: ['all', 'mac-apdfl-samples', 'mac-arm-apdfl-samples', 'aix-apdfl-samples', 'linux-armv8-apdfl-samples', 'sparcsolaris-apdfl-samples', 'linux-apdfl-samples', 'windows-apdfl-samples'], description: 'Run on specific platform')
    }
    agent none
    triggers {
        // From the doc: @midnight actually means some time between 12:00 AM and 2:59 AM.
        // This gives us automatic spreading out of jobs, so they don't cause load spikes.
        cron('@midnight')
    }
    stages {
        stage('Matrix stage') {
            matrix {
                agent {
                    label "${NODE}"
                }
                when { anyOf {
                    expression { params.PLATFORM_FILTER == 'all' }
                    expression { params.PLATFORM_FILTER == env.NODE }
                } }
                axes {
                    axis {
                        name 'NODE'
                        values 'mac-apdfl-samples', 'mac-arm-apdfl-samples', 'linux-armv8-apdfl-samples', 'aix-apdfl-samples', 'sparcsolaris-apdfl-samples', 'windows-apdfl-samples', 'linux-apdfl-samples'
                    }
                    axis {
                        name 'BITS'
                        values '64', '32'
                    }
                }
                excludes {
                    exclude {
                        axis {
                            name 'NODE'
                            values 'mac-apdfl-samples', 'mac-arm-apdfl-samples', 'linux-armv8-apdfl-samples'
                        }
                        axis {
                            name 'BITS'
                            values '32'
                        }
                    }
                }
                environment {
                    CONAN_USER_HOME = "${WORKSPACE}"
                }
                stages {
                    stage('Set-Up Environment') {
                        steps {
                            echo "Set-Up Environment ${NODE}"
                            script {
                                if (isUnix()) {
                                    sh 'LIBPATH="" python3 mkenv.py --verbose'
                                    ENV_LOC[NODE] = sh (
                                        script: 'LIBPATH="" python3 mkenv.py --env-name',
                                        returnStdout: true
                                    ).trim()
                                } else {
                                    bat 'python mkenv.py --verbose'
                                    ENV_LOC[NODE] = bat (
                                        // The @ prevents Windows from echoing the command itself into the stdout,
                                        // which would corrupt the value of the returned data.
                                        script: '@python mkenv.py --env-name',
                                        returnStdout: true
                                    ).trim()
                                }
                            }
                        }
                    }
                    stage('Bootstrap') {
                        steps {
                            echo "Bootstrap ${NODE}"
                            script {
                                if (isUnix()) {
                                    sh """. ${ENV_LOC[NODE]}/bin/activate
                                       unset LIBPATH
                                          invoke bootstrap --bits=${BITS} --update
                                    """
                                } else {
                                    bat """CALL ${ENV_LOC[NODE]}\\Scripts\\activate
                                          invoke bootstrap --bits=${BITS} --update
                                    """
                                }
                            }
                        }
                    }
                    stage('Build') {
                        steps {
                            echo "Build ${NODE}"
                            script {
                                if (isUnix()) {
                                    sh """. ${ENV_LOC[NODE]}/bin/activate
                                       unset LIBPATH
                                          invoke build --bits=${BITS}
                                    """
                                } else {
                                    bat """CALL ${ENV_LOC[NODE]}\\Scripts\\activate
                                          invoke build --bits=${BITS}
                                    """
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
