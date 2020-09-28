FROM ubuntu

ENV TZ=America/Los_Angeles
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ >/etc/timezone

RUN apt-get update && apt-get install -y build-essential gcc git cmake libgtk-3-dev libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && apt-get autoremove -y
RUN useradd -m builder

ADD --chown=builder data /opt/openmic/data
ADD --chown=builder include /opt/openmic/include
ADD --chown=builder src /opt/openmic/src
ADD --chown=builder CMakeLists.txt /opt/openmic/CMakeLists.txt

RUN mkdir -p /opt/openmic/build && chown -R builder /opt/openmic
USER builder

RUN cd /opt/openmic/build && cmake /opt/openmic
ENTRYPOINT ["make", "-C", "/opt/openmic/build"]
