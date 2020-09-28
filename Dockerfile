FROM ubuntu

ENV TZ=America/Los_Angeles
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ >/etc/timezone

RUN apt-get update && apt-get install -y build-essential gcc git cmake libgtk-3-dev && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && apt-get autoremove -y
RUN useradd -m builder

ADD --chown=builder data /opt/openmic/data
ADD --chown=builder include /opt/openmic/include
ADD --chown=builder src /opt/openmic/src
ADD --chown=builder CMakeLists.txt /opt/openmic/CMakeLists.txt

RUN mkdir -p /opt/openmic/build && chown -R builder /opt/openmic
USER builder

RUN cd /opt/openmic/build && cmake /opt/openmic
ENTRYPOINT ["make", "-C", "/opt/openmic/build"]
