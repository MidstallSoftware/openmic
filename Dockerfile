FROM ubuntu

ENV TZ=America/Los_Angeles
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ >/etc/timezone

RUN apt-get update && apt-get install -y build-essential gcc git meson ninja-build nvidia-cuda-toolkit ocl-icd-opencl-dev gettext libgtk-3-dev libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-pulseaudio libncurses5-dev libncursesw5-dev && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && apt-get autoremove -y
RUN useradd -m builder

ADD --chown=builder data /opt/openmic/data
ADD --chown=builder include /opt/openmic/include
ADD --chown=builder po /opt/openmic/po
ADD --chown=builder src /opt/openmic/src
ADD --chown=builder meson.build /opt/openmic/meson.build
ADD --chown=builder meson_options.txt /opt/openmic/meson_options.txt

RUN mkdir -p /opt/openmic/build && chown -R builder /opt/openmic
USER builder

RUN cd /opt/openmic && meson /opt/openmic/build 
ENTRYPOINT ["ninja", "-C", "/opt/openmic/build"]
